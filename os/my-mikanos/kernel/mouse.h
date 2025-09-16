#ifndef KERNEL_MOUSE_H
#define KERNEL_MOUSE_H
#include "pixel_writer.h"


class MouseCursor
{
public:
    MouseCursor(PixelWriter* writer, PixelColor erase_color, Vector2D<int> init_position);
    void move_relative(Vector2D<int> displacement);

private:
    PixelWriter* m_pixelWriter = nullptr;
    PixelColor m_eraseColor;
    Vector2D<int> m_positon;
};


#endif //KERNEL_MOUSE_H