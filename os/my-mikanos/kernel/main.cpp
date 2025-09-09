#include <cstdint>
#include <cstddef>

#include "font.h"
#include "frame_buffer_config.h"
#include "pixel_writer.h"


void *operator new(size_t size, void *buf)
{
    return buf;
}

void operator delete(void* obj) noexcept
{
}

char pixel_writer_buf[sizeof(RGBPixelWriter)];
PixelWriter *pixel_writer;

extern "C" void KernelMain(const FrameBufferConfig *config)
{
    switch (config->pixel_format)
    {
    case kPixelRGBResv8BitPerColor:
        pixel_writer = new(pixel_writer_buf) RGBPixelWriter(config);
        break;

    case kPixelBGRResv8BitPerColor:
        pixel_writer = new(pixel_writer_buf) BGRPixelWriter(config);
    }

    for (int x = 0; x < config->horizontal_resolution; ++x)
    {
        for (int y = 0; y < config->vertical_resolution; ++y)
        {
            pixel_writer->write(x, y, {255, 255, 255});
        }
    }

    for (int x = 100; x < 300; ++x)
    {
        for (int y = 100; y < 200; ++y)
        {
            pixel_writer->write(x, y, {0, 255, 0});
        }
    }

    write_char(*pixel_writer, 50, 50, 'A', {0, 0, 0});
    write_char(*pixel_writer, 58, 50, 'Q', {0, 0, 0});

    while (true)
    {
        __asm__("hlt");
    }
}