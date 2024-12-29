cmake_minimum_required(VERSION 3.0)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} - Wall -Wextra -02")

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# build an executable names 'chip8' from our source files
add_executable(
    chip8
    main.cpp
    chip8.cpp
    platform.cpp
)

# link sdl2 to the chip8 executable
target_link_libraries(chip8 ${SDL2_LIBRARIES})
