#include <iostream>
#include <locale>
#include <memory>
#include <vector>

#include <ccap.h>
#include <ncpp/ncpp.hh>
#include <ncpp/Visual.hh>
#include <spdlog/spdlog.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

#define CCAP_GRAB_MAXIMUM_WAIT_TIME 3000

std::shared_ptr<ccap::VideoFrame> captureCamera(ccap::Provider &cameraProvider)
{
    auto videoFrame = cameraProvider.grab(CCAP_GRAB_MAXIMUM_WAIT_TIME);

    if (!videoFrame)
        throw std::runtime_error("couldn't capture camera");

    return videoFrame;
}

void renderCamera(ncpp::NotCurses *notCurses, std::shared_ptr<ccap::VideoFrame> videoFrame)
{
    auto stdplane = notCurses->get_stdplane();
    std::vector<uint32_t> rgba(videoFrame->width * videoFrame->height);
    for (int y = 0; y < videoFrame->height; y++)
    {
        for (int x = 0; x < videoFrame->width; x++)
        {
            size_t idx = (y * videoFrame->width + (videoFrame->width - x - 1)) * 3;
            size_t rgba_idx = y * videoFrame->width + x;
            uint32_t r = videoFrame->data[0][idx + 2];
            uint32_t g = videoFrame->data[0][idx + 1];
            uint32_t b = videoFrame->data[0][idx];
            rgba[rgba_idx] =
                (r) |
                (g << 8) |
                (b << 16) |
                0xFF000000;
        }
    }
    ncpp::Visual vis(rgba.data(), videoFrame->height, videoFrame->width * sizeof(uint32_t), videoFrame->width);
    ncvisual_options nsvopts{};
    nsvopts.n = stdplane->to_ncplane();
    nsvopts.scaling = NCSCALE_STRETCH;
    vis.blit(&nsvopts);

    notCurses->render();
}

int main(int argc, char const *argv[])
{
    if (!setlocale(LC_ALL, ""))
    {
        spdlog::error("failed to set locale");
        return EXIT_FAILURE;
    }

    ncpp::NotCurses notCurses;
    ccap::Provider cameraProvider;

    cameraProvider.set(ccap::PropertyName::PixelFormatInternal, ccap::PixelFormat::RGB24);

    ccap::setErrorCallback([](ccap::ErrorCode errorCode, std::string_view errorDescription)
                           { spdlog::error(errorDescription); });

    bool running = true;
    ncinput in;

    try
    {
        if (cameraProvider.open())
        {
            while (running)
            {
                auto videoFrame = captureCamera(cameraProvider);
                renderCamera(&notCurses, videoFrame);
                if (notCurses.get(false, &in))
                {
                    if (in.id == 'q')
                        running = false;
                }
            }
        }
        else
            throw std::runtime_error("couldn't open the capture device");
    }
    catch (const std::exception &e)
    {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
