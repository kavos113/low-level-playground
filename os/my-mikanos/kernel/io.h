#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <stdint.h>

extern "C"
{
void IoOut32(uint16_t addr, uint32_t data);
uint32_t IoIn32(uint16_t addr);
}
#endif //KERNEL_IO_H