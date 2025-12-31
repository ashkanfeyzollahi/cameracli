# cameracli

<p align="center">
  <i>📷 A cross-platform terminal-based camera viewer with an interactive menu.</i>
  <br>
  <img alt="GitHub License" src="https://img.shields.io/github/license/ashkanfeyzollahi/cameracli">
</p>

`cameracli` is a C++ command-line application that captures live camera input and renders it directly inside your terminal using a text-based user interface. It provides a simple, interactive control panel for toggling visual effects such as ASCII rendering, grayscale mode, and frame flipping — all in real time.

The project is designed to be lightweight, portable, and fully cross-platform.

## Features

* Live camera capture inside the terminal
* Interactive menu-driven TUI powered by **FTXUI**
* Toggleable rendering options:

  * ASCII mode
  * Grayscale
  * Horizontal and vertical flipping
* Lower memory consumption compared to earlier versions
* Cross-platform support (Linux, macOS, Windows)
* Structured error logging via **spdlog**

## Compiling

This project uses **CMake** for configuration and building.

### Requirements

* C++20 compatible compiler
* CMake (≥ 3.16)
* Dependencies:

  * [FTXUI](https://github.com/ArthurSonzogni/FTXUI) (Cloned when configuring via CMake)
  * [CameraCapture](https://github.com/wysaid/CameraCapture) (Cloned when configuring via CMake)
  * [spdlog](https://github.com/gabime/spdlog) (Needs to be installed)

### Build Steps

```bash
git clone https://github.com/ashkanfeyzollahi/cameracli.git
cd cameracli
mkdir build && cd build
cmake ..
cmake --build .
```

## Acknowledgements

* **CameraCapture** — cross-platform camera capture backend
* **FTXUI** — terminal user interface framework
* **spdlog** — logging library

## Bug Reports

If you encounter a bug or unexpected behavior, please open an issue on GitHub.
Including platform details and reproduction steps is highly appreciated.

## License

This project is licensed under the **MIT License**.
