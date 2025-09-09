#ifndef KERNEL_PIXEL_WRITER_H
#define KERNEL_PIXEL_WRITER_H

#include "frame_buffer_config.h"

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
    uint8_t *pixel_at(int x, int y) const
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

    void write(int x, int y, const PixelColor& c) override;
};

class BGRPixelWriter : public PixelWriter
{
    using PixelWriter::PixelWriter;

    void write(int x, int y, const PixelColor& c) override;
};


#endif //KERNEL_PIXEL_WRITER_H