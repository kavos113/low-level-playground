#include "mouse.h"

namespace
{
constexpr int kMouseCursorWidth = 15;
constexpr int kMouseCursorHeight = 24;
constexpr char mouse_cursor_shape[kMouseCursorHeight][kMouseCursorWidth + 1] = {
    "@              ",
    "@@             ",
    "@.@            ",
    "@..@           ",
    "@...@          ",
    "@....@         ",
    "@.....@        ",
    "@......@       ",
    "@.......@      ",
    "@........@     ",
    "@.........@    ",
    "@..........@   ",
    "@...........@  ",
    "@............@ ",
    "@......@@@@@@@@",
    "@......@       ",
    "@....@@.@      ",
    "@...@ @.@      ",
    "@..@   @.@     ",
    "@.@    @.@     ",
    "@@      @.@    ",
    "@       @.@    ",
    "         @.@   ",
    "         @@@   ",
};

void draw_mouse_cursor(PixelWriter* pixel_writer, Vector2D<int> position)
{
    for (int y = 0; y < kMouseCursorHeight; ++y)
    {
        for (int x = 0; x < kMouseCursorWidth; ++x)
        {
            if (mouse_cursor_shape[y][x] == '@')
            {
                pixel_writer->write(position.x + x, position.y + y, {0, 0, 0});
            }
            else if (mouse_cursor_shape[y][x] == '.')
            {
                pixel_writer->write(position.x + x, position.y + y, {255, 255, 255});
            }
        }
    }
}

void erase_mouse_cursor(PixelWriter* pixel_writer, Vector2D<int> position, PixelColor erase_color)
{
    for (int y = 0; y < kMouseCursorHeight; ++y)
    {
        for (int x = 0; x < kMouseCursorWidth; ++x)
        {
            if (mouse_cursor_shape[y][x] != ' ')
            {
                pixel_writer->write(position.x + x, position.y + y, erase_color);
            }
        }
    }
}
}

MouseCursor::MouseCursor(PixelWriter* writer, PixelColor erase_color, Vector2D<int> init_position)
    : m_pixelWriter(writer), m_eraseColor(erase_color), m_positon(init_position)
{
    draw_mouse_cursor(m_pixelWriter, m_positon);
}

void MouseCursor::move_relative(Vector2D<int> displacement)
{
    erase_mouse_cursor(m_pixelWriter, m_positon, m_eraseColor);
    m_positon += displacement;
    draw_mouse_cursor(m_pixelWriter, m_positon);
}