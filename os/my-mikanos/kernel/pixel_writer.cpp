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

void fill_rect(PixelWriter& writer, const Vector2D<int>& pos, const Vector2D<int>& size, const PixelColor& color)
{
    for (int y = 0; y < size.y; ++y)
    {
        for (int x = 0; x < size.x; ++x)
        {
            writer.write(pos.x + x, pos.y + y, color);
        }
    }
}

void draw_rect(PixelWriter& writer, const Vector2D<int>& pos, const Vector2D<int>& size, const PixelColor& color)
{
    for (int x = 0; x < size.x; ++x)
    {
        writer.write(pos.x + x, pos.y, color);
        writer.write(pos.x + x, pos.y + size.y, color);
    }

    for (int y = 0; y < size.y; ++y)
    {
        writer.write(pos.x, pos.y + y, color);
        writer.write(pos.x + size.x, pos.y + y, color);
    }
}