#include <cstdint>
#include <cstddef>

#include "frame_buffer_config.h"

const uint8_t kFontA[16] = {
    0b00000000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00100100,
    0b00100100,
    0b00100100,
    0b00100100,
    0b01111110,
    0b01000010,
    0b01000010,
    0b01000010,
    0b11100111,
    0b00000000,
    0b00000000,
};

struct PixelColor
{
    uint8_t r, g, b;
};

class PixelWriter
{
public:
    explicit PixelWriter(const FrameBufferConfig* config) : m_config(config) {}
    virtual ~PixelWriter() = default;

    virtual void write(int x, int y, const PixelColor& c) = 0;

protected:
    uint8_t *pixelAt(int x, int y) const
    {
        return m_config->frame_buffer + 4 * (m_config->pixels_per_scan_line * y + x);
    }

private:
    const FrameBufferConfig* m_config;
};

class RGBPixelWriter final : public PixelWriter
{
public:
    using PixelWriter::PixelWriter;

    void write(int x, int y, const PixelColor& c) override
    {
        auto p = pixelAt(x, y);
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
    }
};

class BGRPixelWriter : public PixelWriter
{
    using PixelWriter::PixelWriter;

    void write(int x, int y, const PixelColor& c) override
    {
        auto p = pixelAt(x, y);
        p[0] = c.b;
        p[1] = c.g;
        p[2] = c.r;
    }
};

void writeChar(PixelWriter& writer, int x, int y, char c, const PixelColor& color)
{
    if (c != 'A')
    {
        return;
    }

    for (int dy = 0; dy < 16; ++dy)
    {
        for (int dx = 0; dx < 8; ++dx)
        {
            if ((kFontA[dy] << dx) & 0x80u)
            {
                writer.write(x + dx, y + dy, color);
            }
        }
    }
}

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

    writeChar(*pixel_writer, 50, 50, 'A', {0, 0, 0});
    writeChar(*pixel_writer, 58, 50, 'A', {0, 0, 0});

    while (true)
    {
        __asm__("hlt");
    }
}