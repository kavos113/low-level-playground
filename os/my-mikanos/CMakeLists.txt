# for CLion

cmake_minimum_required(VERSION 4.0)
project(my_mikanos C CXX)

set(CMAKE_C_STANDARD 11)

add_executable(my_mikanos
        MikanLoaderPkg/Main.c
        kernel/main.cpp
        kernel/frame_buffer_config.h
        kernel/elf.h
        kernel/pixel_writer.cpp
        kernel/pixel_writer.h
        kernel/font.cpp
        kernel/font.h
        kernel/console.cpp
        kernel/console.h
)

target_include_directories(my_mikanos PRIVATE
        $ENV{HOME}/edk2/MdePkg/Include
        $ENV{HOME}/edk2/MdePkg/Include/X64
)