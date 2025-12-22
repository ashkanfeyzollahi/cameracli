#include <iostream>
#include <memory>
#include <vector>

#include <ccap.h>
#include <ncurses.h>
#include <spdlog/spdlog.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

static const std::string GSCALE("$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ");

class NCursesContext
{
public:
    NCursesContext();
    ~NCursesContext();

    NCursesContext(const NCursesContext &) = delete;
    NCursesContext &operator=(const NCursesContext &) = delete;
};

NCursesContext::NCursesContext()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    if (has_colors())
        start_color();
}

NCursesContext::~NCursesContext()
{
    endwin();
}

void captureAndRenderFrame(ccap::Provider *cameraProvider)
{
    auto frame = cameraProvider->grab(3000);

    if (frame)
    {
        int termy = getmaxy(stdscr), termx = getmaxx(stdscr);
        std::vector<uint8_t> resized(termx * termy * 3);
        stbir_resize_uint8_srgb(
            frame->data[0], frame->width, frame->height, 0,
            resized.data(), termx, termy, 0, STBIR_BGR);
        for (int y = 0; y < termy; ++y)
        {
            for (int x = 0; x < termx; ++x)
            {
                int idx = (y * termx + x) * 3;

                uint8_t b = resized.at(idx);
                uint8_t g = resized.at(idx + 1);
                uint8_t r = resized.at(idx + 2);

                int gs = (b + g + r) / 3;
                char gsval = GSCALE.at(69 - (gs * 69) / 255);

                mvaddch(y, termx - x, gsval);
            }
        }
        refresh();
    }
}

int main(int argc, char const *argv[])
{
    NCursesContext ncursesContext;

    ccap::Provider cameraProvider;

    cameraProvider.set(ccap::PropertyName::PixelFormatInternal, ccap::PixelFormat::BGR24);

    ccap::setErrorCallback([](ccap::ErrorCode errorCode, std::string_view errorDescription)
                           { spdlog::error(errorDescription); });

    try
    {
        if (cameraProvider.open())
        {
            while (true)
                captureAndRenderFrame(&cameraProvider);
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
