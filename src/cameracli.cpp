#include <iostream>
#include <locale>
#include <memory>
#include <vector>

#include <ccap.h>
#include <ncpp/ncpp.hh>
#include <ncpp/Visual.hh>
#include <spdlog/spdlog.h>

#define CCAP_GRAB_MAXIMUM_WAIT_TIME 3000

std::shared_ptr<ccap::VideoFrame> captureCamera(ccap::Provider &cameraProvider)
{
    auto videoFrame = cameraProvider.grab(CCAP_GRAB_MAXIMUM_WAIT_TIME);

    if (!videoFrame)
        throw std::runtime_error("couldn't capture camera");

    return videoFrame;
}

std::vector<uint32_t> copyVideoFrameData(std::shared_ptr<ccap::VideoFrame> videoFrame)
{
    std::vector<uint32_t> videoFrameDataCopy(videoFrame->width * videoFrame->height);
    memcpy(videoFrameDataCopy.data(), videoFrame->data[0], videoFrame->sizeInBytes);
    return videoFrameDataCopy;
}

void processVideoFrame(std::shared_ptr<ccap::VideoFrame> videoFrame)
{
    std::vector<uint32_t> videoFrameDataCopy = copyVideoFrameData(videoFrame);
    for (uint32_t y = 0; y < videoFrame->height; y++)
    {
        for (uint32_t x = 0; x < videoFrame->width; x++)
        {
            uint32_t idx = (y * videoFrame->width + x);
            uint32_t idx2 = (y * videoFrame->width + (videoFrame->width - x - 1));
            ((uint32_t *)videoFrame->data[0])[idx] = videoFrameDataCopy.at(idx2);
        }
    }
}

ncpp::Plane blitVideoFrame(ncpp::NotCurses &notCurses, std::shared_ptr<ccap::VideoFrame> videoFrame)
{
    auto stdplane = notCurses.get_stdplane();

    ncplane_options cameraPlaneOptions{};

    cameraPlaneOptions.x = 0;
    cameraPlaneOptions.y = 0;
    cameraPlaneOptions.cols = stdplane->get_dim_x();
    cameraPlaneOptions.rows = stdplane->get_dim_y();

    ncpp::Plane cameraPlane(stdplane, cameraPlaneOptions);

    ncpp::Visual vis((uint32_t *)videoFrame->data[0], videoFrame->height, videoFrame->width * sizeof(uint32_t), videoFrame->width);
    ncvisual_options nsvopts{};
    nsvopts.n = cameraPlane.to_ncplane();
    nsvopts.scaling = NCSCALE_STRETCH;
    vis.blit(&nsvopts);

    return cameraPlane;
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
    cameraProvider.set(ccap::PropertyName::PixelFormatOutput, ccap::PixelFormat::RGBA32);

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
                processVideoFrame(videoFrame);
                auto videoFramePlane = blitVideoFrame(notCurses, videoFrame);

                notCurses.render();
                notCurses.get(false, &in);

                if (in.id == 'q')
                    running = false;
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
