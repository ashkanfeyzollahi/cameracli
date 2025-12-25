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

void captureAndRenderFrame(ncpp::NotCurses *notCurses, ccap::Provider *cameraProvider)
{
    auto frame = cameraProvider->grab(3000);

    auto stdplane = notCurses->get_stdplane();
    unsigned int termw, termh;
    stdplane->get_dim(&termh, &termw);

    if (frame)
    {
        std::vector<uint8_t> resized(termw * termh * 3);
        stbir_resize_uint8_srgb(
            frame->data[0], frame->width, frame->height, 0,
            resized.data(), termw, termh, 0, STBIR_BGR);
        std::vector<uint32_t> rgba(termw * termh);
        for (int y = 0; y < termh; y++)
        {
            for (int x = 0; x < termw; x++)
            {
                size_t resized_idx = (y * termw + (termw - x - 1)) * 3;

                size_t rgba_idx = y * termw + x;

                uint32_t r = resized[resized_idx + 2];
                uint32_t g = resized[resized_idx + 1];
                uint32_t b = resized[resized_idx];

                rgba[rgba_idx] =
                    (r) |
                    (g << 8) |
                    (b << 16) |
                    0xFF000000;
            }
        }

        ncpp::Visual vis(rgba.data(), termh, termw * sizeof(uint32_t), termw);
        ncvisual_options nsvopts{};
        nsvopts.n = stdplane->to_ncplane();
        nsvopts.scaling = NCSCALE_STRETCH;
        vis.blit(&nsvopts);
    }

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

    cameraProvider.set(ccap::PropertyName::PixelFormatInternal, ccap::PixelFormat::BGR24);

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
                captureAndRenderFrame(&notCurses, &cameraProvider);
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
