# cameracli

<p align="center"><i>📷 A terminal-based real-time camera viewer that renders live video as ASCII art.</i>
    <br>
    <img alt="GitHub License" src="https://img.shields.io/github/license/ashkanfeyzollahi/cameracli">
</p>

**cameracli** is a lightweight command-line application that captures live camera frames and renders them directly in the terminal using ASCII grayscale characters.
It combines real-time video capture with terminal graphics to create a retro, text-based camera experience.

Built using **modern C++**, **Meson** for building, **ncurses** for terminal rendering, **ccap** for camera capture, **stb_image_resize** for image scaling, and **spdlog** for logging, cameracli demonstrates efficient multimedia processing entirely inside a terminal.

---

## Features

* Real-time camera capture via **ccap**
* ASCII grayscale rendering in the terminal
* Automatic resizing to current terminal dimensions
* ncurses-based rendering for smooth updates
* Simple RAII-based ncurses context management
* BGR24 pixel format support
* Minimal and fast rendering loop
* Modern C++ design (RAII, STL containers)
* Clean Meson-based build system

---

## Quick Reference

**Binary name:** `cameracli`
**Run command:** `./cameracli`

After building the project, simply run:

```bash
./cameracli
```

Make sure your terminal supports ncurses and your system has a working camera device.

---

## Rendering Basics

### Grayscale Mapping

Each pixel is converted to grayscale using the average of RGB values:

```
gray = (R + G + B) / 3
```

The grayscale value is mapped to a character from the following scale:

```
$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,"^`'.
```

Darker pixels use denser characters, while brighter pixels use lighter ones.

---

## How It Works

### Frame Capture

* A `ccap::Provider` instance opens the default camera device
* Frames are grabbed with a timeout of 3000 ms
* Pixel format is set to **BGR24** for compatibility

### Resizing

* Each frame is resized to match the terminal size
* Uses **stb_image_resize** with sRGB-aware resizing
* Output buffer matches terminal rows × columns

### Rendering

* Each resized pixel is converted to a grayscale character
* Characters are rendered using `mvaddch`
* Screen refreshes continuously for real-time output

---

## Running the Project

### Requirements

* **C++17** compatible compiler
* **Meson** and **Ninja**
* **ncurses**
* **ccap**
* **spdlog**
* A working camera device (e.g., `/dev/video0` on Linux)

---

### Build Instructions

1. Clone the repository:

```bash
git clone https://github.com/ashkanfeyzollahi/cameracli
```

2. Enter the project directory:

```bash
cd cameracli
```

3. Configure the build:

```bash
meson setup build
```

4. Compile:

```bash
meson compile -C build
```

5. Run:

```bash
./build/cameracli
```

---

## Technical Breakdown

* **NCursesContext**

  * RAII wrapper for initializing and shutting down ncurses
  * Ensures terminal state is restored on exit

* **Camera Provider**

  * Uses `ccap::Provider` to open and manage the camera
  * Frame grabbing handled in a tight render loop

* **Image Processing**

  * Raw camera frames resized using `stbir_resize_uint8_srgb`
  * Efficient buffer reuse via `std::vector<uint8_t>`

* **Rendering Loop**

  * Terminal dimensions queried every frame
  * ASCII characters rendered per pixel
  * `refresh()` updates the terminal

---

## Configuration

Currently, cameracli uses sensible defaults:

* Camera: default system camera
* Pixel format: `BGR24`
* Frame timeout: 3000 ms
* Terminal size: auto-detected per frame

Future improvements may include CLI flags for device selection and frame rate control.

---

## Error Handling

* Camera errors are reported via **ccap error callbacks**
* All errors are logged using **spdlog**
* ncurses cleanup is guaranteed via RAII, even on failure

Example error handling callback:

```cpp
ccap::setErrorCallback([](ccap::ErrorCode code, std::string_view desc) {
    spdlog::error(desc);
});
```

---

## Architecture Overview

### Core Components

* **Main Loop** – Captures frames and triggers rendering
* **NCursesContext** – Manages terminal lifecycle
* **Renderer** – Converts image data to ASCII output
* **Resizer** – Scales frames to terminal resolution

### Performance Notes

* Minimal allocations per frame
* Direct buffer access for rendering
* Efficient ASCII mapping with constant-time lookup

---

## Acknowledgements

Thanks to:

* **ncurses** for terminal rendering
* **ccap** for camera capture abstraction
* **stb_image_resize** for fast image scaling
* **spdlog** for structured logging

---

## Bug Reports

Found a bug or want to suggest a feature?
[Open an issue](https://github.com/ashkanfeyzollahi/cameracli/issues) on GitHub.

