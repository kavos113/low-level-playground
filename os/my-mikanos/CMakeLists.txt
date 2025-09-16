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
        kernel/newlib_support.c
        kernel/pci.cpp
        kernel/pci.h
        kernel/io.h
        kernel/error.h
        kernel/usb/device.cpp
        kernel/usb/device.hpp
        kernel/usb/memory.cpp
        kernel/usb/memory.hpp
        kernel/usb/classdriver/base.cpp
        kernel/usb/classdriver/base.hpp
        kernel/usb/classdriver/hid.cpp
        kernel/usb/classdriver/hid.hpp
        kernel/usb/classdriver/keyboard.cpp
        kernel/usb/classdriver/keyboard.hpp
        kernel/usb/classdriver/mouse.cpp
        kernel/usb/classdriver/mouse.hpp
        kernel/usb/xhci/device.hpp
        kernel/usb/xhci/device.cpp
        kernel/usb/xhci/devmgr.cpp
        kernel/usb/xhci/devmgr.hpp
        kernel/usb/xhci/port.cpp
        kernel/usb/xhci/port.hpp
        kernel/usb/xhci/registers.cpp
        kernel/usb/xhci/registers.hpp
        kernel/usb/xhci/ring.cpp
        kernel/usb/xhci/ring.hpp
        kernel/usb/xhci/speed.hpp
        kernel/usb/xhci/trb.cpp
        kernel/usb/xhci/trb.hpp
        kernel/usb/xhci/xhci.cpp
        kernel/usb/xhci/xhci.hpp
        kernel/mouse.cpp
        kernel/mouse.h
        kernel/logger.cpp
        kernel/logger.h
        kernel/register.h
        kernel/libcxx_support.cpp
)

target_include_directories(my_mikanos PRIVATE
        $ENV{HOME}/edk2/MdePkg/Include
        $ENV{HOME}/edk2/MdePkg/Include/X64
        kernel
)