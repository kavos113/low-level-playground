#ifndef KERNEL_FONT_H
#define KERNEL_FONT_H

#include "pixel_writer.h"

void write_char(PixelWriter& writer, int x, int y, char c, const PixelColor& color);
void write_string(PixelWriter& writer, int x, int y, const char* s, const PixelColor& color);

#endif //KERNEL_FONT_H