#include "pixel_writer.h"

void RGBPixelWriter::write(int x, int y, const PixelColor& c)
{
    auto p = pixel_at(x, y);
    p[0] = c.r;
    p[1] = c.g;
    p[2] = c.b;
}

void BGRPixelWriter::write(int x, int y, const PixelColor& c)
{
    auto p = pixel_at(x, y);
    p[0] = c.b;
    p[1] = c.g;
    p[2] = c.r;
}
