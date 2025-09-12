#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>

#include "console.h"
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

char console_buf[sizeof(Console)];
Console *console;

int printk(const char *format, ...)
{
    va_list ap;
    char s[1024];

    va_start(ap, format);
    int result = vsprintf(s, format, ap);
    va_end(ap);

    console->put_string(s);
    return result;
}

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

    console = new(console_buf) Console(pixel_writer, {255, 255, 255}, {0, 0, 0});

    for (int x = 0; x < config->horizontal_resolution; ++x)
    {
        for (int y = 0; y < config->vertical_resolution; ++y)
        {
            pixel_writer->write(x, y, {255, 255, 255});
        }
    }

    for (int i = 0; i < 28; ++i)
    {
        printk("printk: %d\n", i);
    }

    while (true)
    {
        __asm__("hlt");
    }
}