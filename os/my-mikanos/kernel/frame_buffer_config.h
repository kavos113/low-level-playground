#ifndef KERNEL_FRAME_BUFFER_CONFIG_H
#define KERNEL_FRAME_BUFFER_CONFIG_H

#include <stdint.h>

enum PixelFormat
{
    kPixelRGBResv8BitPerColor,
    kPixelBGRResv8BitPerColor,
};

struct FrameBufferConfig
{
    uint8_t* frame_buffer;
    uint32_t pixels_per_scan_line;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    enum PixelFormat pixel_format;
};

#endif //KERNEL_FRAME_BUFFER_CONFIG_H