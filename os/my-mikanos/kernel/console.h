#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H
#include "pixel_writer.h"


class Console
{
public:
    static constexpr int kRows = 25, kColumns = 80;

    Console(PixelWriter* writer, const PixelColor& fg_color, const PixelColor& bg_color);

    void put_string(const char* s);

private:
    void new_line();

    PixelWriter* m_writer;
    const PixelColor m_fgColor, m_bgColor;
    char m_buffer[kRows][kColumns + 1];
    int m_cursorRow, m_cursorColumn;
};


#endif //KERNEL_CONSOLE_H