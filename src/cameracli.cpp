#include <chrono>
using namespace std::chrono_literals;
#include <iostream>
#include <locale>
#include <memory>
#include <vector>

#include <ccap.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <spdlog/spdlog.h>
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb/stb_image_resize2.h>

#define CCAP_GRAB_MAXIMUM_WAIT_TIME 3000

namespace cameracli {
class IntervalTimer {
public:
    IntervalTimer(ftxui::ScreenInteractive &screen, std::chrono::milliseconds interval)
        : running_(true)
        , thread_([&, interval] {
            while (running_) {
                std::this_thread::sleep_for(interval);
                screen.PostEvent(ftxui::Event::Custom);
            }
        }) {}

    ~IntervalTimer() {
        running_ = false;
        thread_.join();
    }

private:
    std::atomic<bool> running_;
    std::thread thread_;
};

struct Pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class Frame {
public:
    std::vector<Pixel> pixels;
    int height, width;
    Frame(std::vector<Pixel> pixels, int width, int height);
    Frame(std::shared_ptr<ccap::VideoFrame> videoFrame);
    Pixel at(int x, int y);
    Frame copy();
    void flip(bool flipX, bool flipY);
    void resize(int width, int height);
    void setPixel(Pixel pixel, int x, int y);
};

Frame::Frame(std::vector<Pixel> pixels, int width, int height) {
    this->pixels = pixels;
    this->width = width;
    this->height = height;
}

Frame::Frame(std::shared_ptr<ccap::VideoFrame> videoFrame) {
    height = videoFrame->height;
    width = videoFrame->width;
    pixels = std::vector<Pixel>(width * height);
    memcpy(pixels.data(), videoFrame->data[0], width * height * 3);
}

Pixel Frame::at(int x, int y) { return pixels.at(y * width + x); }

Frame captureCamera(ccap::Provider &cameraProvider) {
    auto videoFrame = cameraProvider.grab(CCAP_GRAB_MAXIMUM_WAIT_TIME);

    if (!videoFrame)
        throw std::runtime_error("couldn't capture camera");

    return Frame(videoFrame);
}

Frame Frame::copy() {
    std::vector<Pixel> frameDataCopy(width * height);
    memcpy(frameDataCopy.data(), pixels.data(), width * height * 3);
    return Frame(frameDataCopy, width, height);
}

void Frame::flip(bool flipX, bool flipY) {
    if (!flipX && !flipY)
        return;
    auto frameCopy = copy();
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int otherX = flipX ? width - x - 1 : x;
            int otherY = flipY ? height - y - 1 : y;
            setPixel(frameCopy.at(otherX, otherY), x, y);
        }
    }
}

void Frame::resize(int width, int height) {
    std::vector<Pixel> resizedFrame(width * height);
    stbir_resize_uint8_srgb((unsigned char *)pixels.data(), this->width, this->height, 0,
                            (unsigned char *)resizedFrame.data(), width, height, 0, STBIR_RGB);
    pixels = resizedFrame;
    this->width = width;
    this->height = height;
}

void Frame::setPixel(Pixel pixel, int x, int y) { pixels[y * width + x] = pixel; }

struct FrameOptions {
    bool ascii, flipX, flipY, grayscale;
};

std::string pixelToASCII(Pixel pixel) {
    static std::string grayscaleCharset =
        "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.          ";
    int avg = (pixel.red + pixel.green + pixel.blue) / 3;
    return std::string({grayscaleCharset.at(78 - avg * 78 / 255)});
}

ftxui::Element renderFrame(ccap::Provider &cameraProvider, int width, int height,
                           FrameOptions frameOptions) {
    auto frame = captureCamera(cameraProvider);
    frame.resize(width, frameOptions.ascii ? height : height * 2);
    frame.flip(!frameOptions.flipX, frameOptions.flipY);

    ftxui::Elements rows;

    if (frameOptions.ascii) {
        for (int y = 0; y < frame.height; y++) {
            ftxui::Elements row;
            for (int x = 0; x < frame.width; x++) {
                auto pixel = frame.at(x, y);
                if (frameOptions.grayscale)
                    row.push_back(ftxui::text(pixelToASCII(pixel)));
                else {
                    auto color = ftxui::Color::RGB(pixel.red, pixel.green, pixel.blue);
                    row.push_back(ftxui::text("$") | ftxui::color(color));
                }
            }
            rows.push_back(ftxui::hbox(row));
        }
    } else {
        for (int y = 0; y < frame.height; y += 2) {
            ftxui::Elements row;
            for (int x = 0; x < frame.width; x++) {
                auto topPixel = frame.at(x, y);
                auto bottomPixel = frame.at(x, y + 1);
                uint8_t topRed = topPixel.red, topGreen = topPixel.green, topBlue = topPixel.blue;
                if (frameOptions.grayscale) {
                    uint8_t avg = (topRed + topGreen + topBlue) / 3;
                    topRed = avg;
                    topGreen = avg;
                    topBlue = avg;
                }
                ftxui::Color top = ftxui::Color::RGB(topRed, topGreen, topBlue);

                uint8_t bottomRed = bottomPixel.red, bottomGreen = bottomPixel.green,
                        bottomBlue = bottomPixel.blue;
                if (frameOptions.grayscale) {
                    uint8_t avg = (bottomRed + bottomGreen + bottomBlue) / 3;
                    bottomRed = avg;
                    bottomGreen = avg;
                    bottomBlue = avg;
                }
                ftxui::Color bottom = ftxui::Color::RGB(bottomRed, bottomGreen, bottomBlue);
                row.push_back(ftxui::text("▀") | ftxui::color(top) | ftxui::bgcolor(bottom));
            }
            rows.push_back(ftxui::hbox(row));
        }
    }

    return ftxui::vbox(rows) | ftxui::borderRounded;
}

void run(int argc, char const *argv[]) {
    if (!setlocale(LC_ALL, ""))
        throw std::runtime_error("failed to set locale");

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    ccap::Provider cameraProvider;

    cameraProvider.set(ccap::PropertyName::PixelFormatInternal, ccap::PixelFormat::RGB24);
    cameraProvider.set(ccap::PropertyName::PixelFormatOutput, ccap::PixelFormat::RGB24);

    ccap::setErrorCallback([](ccap::ErrorCode errorCode, std::string_view errorDescription) {
        spdlog::error(errorDescription);
    });

    IntervalTimer timer(screen, 1ms);

    struct FrameOptions frameOptions{};

    std::string asciiCheckboxLabel = "ASCII";
    auto asciiCheckbox = ftxui::Checkbox(&asciiCheckboxLabel, &frameOptions.ascii);

    std::string flipXCheckboxLabel = "Flip X";
    auto flipXCheckbox = ftxui::Checkbox(&flipXCheckboxLabel, &frameOptions.flipX);

    std::string flipYCheckboxLabel = "Flip Y";
    auto flipYCheckbox = ftxui::Checkbox(&flipYCheckboxLabel, &frameOptions.flipY);

    std::string grayscaleCheckboxLabel = "Grayscale";
    auto grayscaleCheckbox = ftxui::Checkbox(&grayscaleCheckboxLabel, &frameOptions.grayscale);

    auto checkboxListLayout = ftxui::Container::Vertical(
        {asciiCheckbox, flipXCheckbox, flipYCheckbox, grayscaleCheckbox});
    auto checkboxListRenderer = ftxui::Renderer(checkboxListLayout, [&] {
        return ftxui::vbox({asciiCheckbox->Render(), flipXCheckbox->Render(),
                            flipYCheckbox->Render(), grayscaleCheckbox->Render()});
    });

    std::string quitButtonLabel = "Quit";
    auto quitButton = ftxui::Button(&quitButtonLabel, screen.ExitLoopClosure());

    auto controlPanelLayout = ftxui::Container::Vertical({checkboxListRenderer, quitButton});
    auto controlPanelRenderer = ftxui::Renderer(controlPanelLayout, [&] {
        return ftxui::vbox({checkboxListRenderer->Render(),
                            ftxui::emptyElement() | ftxui::flex_grow, quitButton->Render()}) |
               ftxui::size(ftxui::WIDTH, ftxui::Constraint::GREATER_THAN, 30) |
               ftxui::borderRounded;
    });

    int frameWidth = 0, frameHeight = 0;
    auto frameRenderer = ftxui::Renderer([&] {
        return renderFrame(cameraProvider, frameWidth, frameHeight, frameOptions) |
               ftxui::flex_grow;
    });

    auto layout = ftxui::Container::Horizontal({frameRenderer, controlPanelRenderer});
    auto component = ftxui::Renderer(layout, [&] {
        auto controlPanel = controlPanelRenderer->Render();

        auto screenWidth = screen.dimx(), screenHeight = screen.dimy();
        auto controlPanelRequirement = controlPanel->requirement();

        frameWidth = screenWidth - 33;
        frameHeight = screenHeight;

        auto frame = frameRenderer->Render();

        return ftxui::hbox({frame, controlPanel});
    });
    quitButton->Render();

    if (cameraProvider.open())
        screen.Loop(component);
    else
        throw std::runtime_error("couldn't open the capture device");
}
} // namespace cameracli

int main(int argc, char const *argv[]) {
    try {
        cameracli::run(argc, argv);
    } catch (const std::exception &e) {
        spdlog::error(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
