cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(USB VERSION 0.0.1 LANGUAGES CXX)

find_package(BoilingHotWater REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_package(TeaCup REQUIRED)

FILE(GLOB_RECURSE USB_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE USB_HEADERS "src/*.hpp" "src/*.h")

add_executable(usb ${USB_SOURCES} ${USB_HEADERS})

target_link_libraries(usb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libusb/static/libusb-1.0.lib BoilingHotWater::BoilingHotWater TeaCup::TeaCup)