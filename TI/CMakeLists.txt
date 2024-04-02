cmake_minimum_required(VERSION 3.28.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(USB VERSION 0.0.1 LANGUAGES CXX)

find_package(BoilingHotWater REQUIRED)
find_package(TeaCup REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

FILE(GLOB_RECURSE USB_SOURCES "src/*.cpp")
FILE(GLOB_RECURSE USB_HEADERS "src/*.hpp" "src/*.h")

message(STATUS "USB_SOURCES: ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libusb/include/libusb.h")

add_executable(usb ${USB_SOURCES} ${USB_HEADERS} ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libusb/include/libusb.h)

target_link_libraries(usb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libusb/static/libusb-1.0.lib BoilingHotWater::BoilingHotWater TeaCup::TeaCup)

target_include_directories(usb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libusb/include)