cmake_minimum_required(VERSION 3.10)

project(SevenSegmentProject VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Toolchain root (adjust if needed)
set(TOOLCHAIN_ROOT $ENV{HOME}/x-tools/aarch64-rpi3-linux-gnu)

# Compilers
set(CMAKE_C_COMPILER   ${TOOLCHAIN_ROOT}/bin/aarch64-rpi3-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/aarch64-rpi3-linux-gnu-g++)
# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Static library with correct lowercase filenames
add_library(mylib STATIC
    src/gpio.cpp
    src/stream.cpp
    src/istream.cpp
    src/ostream.cpp
    src/sevensegment.cpp
)

target_include_directories(mylib PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
)

# Main executable
add_executable(program main.cpp)

# Link with the library
target_link_libraries(program mylib)