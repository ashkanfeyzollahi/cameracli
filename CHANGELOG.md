## [2.1.1] - 2026-01-05

### 🐛 Bug Fixes

- Fix y dimensional pixel overflow

### 💼 Other

- Bump patch version
## [2.1.0] - 2025-12-31

### 🚀 Features

- *(ascii)* Extend grayscale charset for better quality

### 💼 Other

- *(version)* Bump minor version
## [2.0.1] - 2025-12-31

### 🐛 Bug Fixes

- Proper frame resizing

### ⚙️ Miscellaneous Tasks

- *(version)* Bump patch version
## [2.0.0] - 2025-12-31

### 🚀 Features

- *(ui)* [**breaking**] Migrate to ftxui and add a menu

### 💼 Other

- *(cpp_std)* Change c++ std to 20
- Migrate from meson to cmake

### 📚 Documentation

- Modify README.md to reflect new version

### ⚙️ Miscellaneous Tasks

- Remove subprojects
- *(formatting)* Add a clang-format config

### ◀️ Revert

- *(stb_image_resize2)* Revert remove stb_image_resize2 header
## [1.1.0] - 2025-12-26

### 🚀 Features

- *(video)* Add frame mirroring and refactor blit pipeline

### 💼 Other

- *(stb_image_resize2)* Remove include_directories external statement

### 🚜 Refactor

- *(pixelformat)* Change internal pixel format to RGB24
- *(utils)* Separate camera capture util with renderer
- *(rgba-conversion)* Remove rgba conversion implementation and let ccap do it

### 📚 Documentation

- *(readme)* Replace INFO with TIP block

### ⚙️ Miscellaneous Tasks

- *(stb_image_resize2)* Remove stb header
- *(stb_image_resize2)* Remove stb header file
## [1.0.0] - 2025-12-25

### 💼 Other

- *(deps)* Replace ncurses with notcurses in meson

### 🚜 Refactor

- *(rendering)* Switch from ncurses to notcurses and drop ASCII renderer

### 📚 Documentation

- *(readme)* Update for notcurses backend and removal of ASCII mode
## [0.1.0] - 2025-12-25

### 📚 Documentation

- Add LICENSE

### ⚙️ Miscellaneous Tasks

- Init
- *(changelog)* Add git-cliff configuration
