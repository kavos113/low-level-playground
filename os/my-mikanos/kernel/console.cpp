#include "console.h"

#include <cstring>
#include "font.h"

Console::Console(PixelWriter* writer, const PixelColor& fg_color, const PixelColor& bg_color)
    : m_writer(writer), m_fgColor(fg_color), m_bgColor(bg_color), m_buffer{}, m_cursorRow(0), m_cursorColumn(0)
{
}

void Console::put_string(const char* s)
{
    while (*s)
    {
        if (*s == '\n')
        {
            new_line();
        }
        else if (m_cursorColumn < kColumns - 1)
        {
            write_char(*m_writer, 8 * m_cursorColumn, 16 * m_cursorRow, *s, m_fgColor);
            m_buffer[m_cursorRow][m_cursorColumn] = *s;

            m_cursorColumn++;
        }

        s++;
    }
}

void Console::new_line()
{
    m_cursorColumn = 0;

    if (m_cursorRow < kRows - 1)
    {
        m_cursorRow++;
    }
    else // scroll
    {
        // fill
        for (int y = 0; y < 16 * kRows; ++y)
        {
            for (int x = 0; x < 8 * kColumns; ++x)
            {
                m_writer->write(x, y, m_bgColor);
            }
        }

        for (int r = 0; r < kRows - 1; ++r)
        {
            memcpy(m_buffer[r], m_buffer[r + 1], kColumns + 1);
            write_string(*m_writer, 0, 16 * r, m_buffer[r], m_fgColor);
        }
        memset(m_buffer[kRows - 1], 0, kColumns + 1);
    }
}