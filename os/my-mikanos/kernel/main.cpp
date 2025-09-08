#include "frame_buffer_config.h"

struct PixelColor
{
    uint8_t r, g, b;
};

int WritePixel(const FrameBufferConfig& config, int x, int y, const PixelColor& c)
{
    const int pixel_position = config.pixels_per_scan_line * y + x;

    switch (config.pixel_format)
    {
    case kPixelRGBResv8BitPerColor:
    {
        uint8_t *p = &config.frame_buffer[4 * pixel_position];
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
    }
    case kPixelBGRResv8BitPerColor:
    {
        uint8_t *p = &config.frame_buffer[4 * pixel_position];
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
    }
    }

    return 0;
}

extern "C" void KernelMain(const FrameBufferConfig& config)
{
    for (int x = 0; x < config.horizontal_resolution; ++x)
    {
        for (int y = 0; y < config.vertical_resolution; ++y)
        {
            WritePixel(config, x, y, {255, 255, 255});
        }
    }

    for (int x = 100; x < 300; ++x)
    {
        for (int y = 100; y < 200; ++y)
        {
            WritePixel(config, x, y, {0, 255, 0});
        }
    }

    while (true)
    {
        __asm__("hlt");
    }
}