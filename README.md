# cameracli

<p align="center">
  <i>📷 A terminal-based real-time camera viewer using Notcurses for high-performance, full-color rendering.</i>
  <br>
  <img alt="GitHub License" src="https://img.shields.io/github/license/ashkanfeyzollahi/cameracli">
</p>

**cameracli** is a lightweight command-line application that captures live camera frames and renders them directly inside the terminal using **Notcurses visuals**.

Instead of traditional ASCII rendering, cameracli leverages **pixel-accurate RGBA blitting** to display real-time video output in modern terminals that support Notcurses, providing a smooth and visually rich terminal camera experience. (version *v0.1.0* uses ASCII rendering if you're interested!)

Built using **modern C++**, **Meson** for building, **Notcurses (ncpp)** for rendering, **ccap** for camera capture, **stb_image_resize** for high-quality image scaling, and **spdlog** for logging.

---

## Features

* Real-time camera capture via **ccap**
* Full-color terminal rendering using **Notcurses visuals**
* Automatic resizing to current terminal dimensions
* High-quality sRGB-aware image resizing
* BGR24 → RGBA pixel conversion
* Minimal rendering latency
* Keyboard input handling (`q` to quit)
* Modern C++ design with RAII
* Clean Meson-based build system

---

## Quick Reference

**Binary name:** `cameracli`  
**Run command:** `./cameracli`  

After building the project, simply run:

```bash
./cameracli
````

Press **`q`** at any time to exit the application.

> [!WARNING]
> Requires a terminal with **Notcurses support** (e.g. Kitty, Alacritty, WezTerm) or a terminal that supports `xterm-256color`.

> [!TIP]
> Version *v0.1.0* doesn't require any fancy terminal and works in almost all terminals that aren't `$TERM=dumb`.

---

## How It Works

### Camera Capture

* A `ccap::Provider` instance opens the default system camera
* Frames are captured with a timeout of **3000 ms**
* Pixel format is explicitly set to **BGR24**
* Errors are reported via **ccap error callbacks**

---

### Terminal and Rendering Context

* Uses **Notcurses (ncpp)** instead of ncurses (version *v0.1.0* uses **ncurses**)
* `ncpp::NotCurses` manages terminal initialization and teardown
* The standard plane (`stdplane`) is used as the render target
* Terminal dimensions are queried **every frame** to stay responsive to resizing

---

### Image Resizing

* Raw camera frames are resized to match terminal dimensions
* Uses **stb_image_resize v2**
* sRGB-aware resizing for improved color accuracy

---

### Pixel Format Conversion

* Camera frames arrive as **BGR24**
* Resized pixels are converted to **RGBA32**
* Alpha channel is set to fully opaque (`0xFF`)
* Horizontal flipping is applied during conversion

---

### Rendering Pipeline

* RGBA buffer is wrapped in an `ncpp::Visual`
* Visual is blitted directly to the standard plane
* Scaling mode: `NCSCALE_STRETCH`
* Rendering is finalized with `notcurses.render()`

---

## Controls

| Key | Action           |
| --- | ---------------- |
| `q` | Quit application |

Keyboard input is handled using Notcurses input events.

---

## Running the Project

### Requirements

* **C++17** compatible compiler
* **Meson** and **Ninja**
* **Notcurses** (with C++ bindings)
* **ccap**
* **spdlog**
* A working camera device
  *Example: `/dev/video0` on Linux*

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

### Core Components

* **Main Loop**

  * Captures frames
  * Renders visuals
  * Handles keyboard input

* **Camera Provider**

  * Wraps camera access via `ccap::Provider`
  * Manages pixel format and frame grabbing

* **Renderer**

  * Converts BGR → RGBA
  * Creates Notcurses visuals
  * Handles blitting and scaling

* **Resizer**

  * Uses `stb_image_resize` for high-quality scaling
  * Matches output resolution to terminal size

---

## Error Handling

* Camera-related errors reported via **ccap error callbacks**
* Runtime errors logged with **spdlog**
* All failures result in clean program termination

---

## Performance Notes

* Minimal allocations per frame
* Efficient buffer reuse via `std::vector`
* Direct memory access for pixel conversion
* No intermediate ASCII conversion overhead
* Terminal resizing handled dynamically

---

## Future Improvements

* CLI flags for:

  * Camera selection
  * Resolution scaling
  * Frame rate limiting
* Optional grayscale / ASCII rendering mode

---

## Acknowledgements

Thanks to:

* **Notcurses** for high-performance terminal graphics
* **ccap** for camera capture abstraction
* **stb_image_resize** for fast and accurate image scaling
* **spdlog** for structured logging

---

## Bug Reports

Found a bug or want to suggest a feature?
[Open an issue](https://github.com/ashkanfeyzollahi/cameracli/issues) on GitHub.

