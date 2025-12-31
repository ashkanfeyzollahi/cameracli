cmake_minimum_required(VERSION 3.16)

project(
    cameracli
    VERSION 2.0.0
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(ccap
    GIT_REPOSITORY https://github.com/wysaid/CameraCapture
    GIT_TAG v1.5.0
)
FetchContent_MakeAvailable(ccap)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(spdlog REQUIRED)

add_executable(cameracli src/cameracli.cpp)
target_include_directories(cameracli PRIVATE
    external
)
target_link_libraries(cameracli PRIVATE
    ccap
    ftxui::component
    ftxui::dom
    ftxui::screen
    spdlog::spdlog
)
