#ifndef KERNEL_PCI_H
#define KERNEL_PCI_H
#include <array>
#include <cstdint>

#include "error.h"

namespace pci
{
constexpr uint16_t kConfigAddress = 0x0cf8;
constexpr uint16_t kConfigData = 0x0cfc;

void write_address(uint32_t address);
void write_data(uint32_t value);
uint32_t read_data();
uint16_t read_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
uint16_t read_device_id(uint8_t bus, uint8_t device, uint8_t function);
uint8_t read_header_type(uint8_t bus, uint8_t device, uint8_t function);
uint32_t read_class_code(uint8_t bus, uint8_t device, uint8_t function);
uint32_t read_bus_numbers(uint8_t bus, uint8_t device, uint8_t function); // for header type 1
bool is_single_function_device(uint8_t header_type);

struct Device
{
    uint8_t bus, device, function, header_type;
};

inline std::array<Device, 32> devices;
inline int num_device;

Error scan_all_bus();
}

#endif //KERNEL_PCI_H