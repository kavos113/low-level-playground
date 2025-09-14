#include "pci.h"

#include "io.h"

namespace
{

using namespace pci;

uint32_t make_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg_offset)
{
    auto shl = [](uint32_t x, unsigned int bits)
    {
        return x << bits;
    };

    return shl(1, 31)
        | shl(bus, 16)
        | shl(device, 11)
        | shl(function, 8)
        | (reg_offset & 0xfcu); // force 0 in 2-low-bits
}

Error add_device(uint8_t bus, uint8_t device, uint8_t function, uint8_t header_type)
{
    if (num_device == devices.size())
    {
        return Error::Code::FULL;
    }

    devices[num_device] = Device{bus, device, function, header_type};
    num_device++;
    return Error::Code::SUCCESS;
}

Error scan_bus(uint8_t bus);

Error scan_function(uint8_t bus, uint8_t device, uint8_t function)
{
    auto header_type = read_header_type(bus, device, function);
    if (auto err = add_device(bus, device, function, header_type))
    {
        return err;
    }

    auto class_code = read_class_code(bus, device, function);
    uint8_t base_class = (class_code >> 24) & 0xffu;
    uint8_t sub_class = (class_code >> 16) & 0xffu;

    if (base_class == 0x06u && sub_class == 0x04u)
    {
        auto bus_numbers = read_bus_numbers(bus, device, function);
        uint8_t secondary_bus = (bus_numbers >> 8) & 0xffu;
        return scan_bus(secondary_bus);
    }

    return Error::Code::SUCCESS;
}

Error scan_device(uint8_t bus, uint8_t device)
{
    if (auto err = scan_function(bus, device, 0))
    {
        return err;
    }

    if (is_single_function_device(read_header_type(bus, device, 0)))
    {
        return Error::Code::SUCCESS;
    }

    for (uint8_t function = 1; function < 8; ++function)
    {
        if (read_vendor_id(bus, device, function) == 0xffffu)
        {
            continue;
        }

        if (auto err = scan_function(bus, device, function))
        {
            return err;
        }
    }

    return Error::Code::SUCCESS;
}

Error scan_bus(uint8_t bus)
{
    for (uint8_t device = 0; device < 32; device++)
    {
        if (read_vendor_id(bus, device, 0) == 0xffffu)
        {
            continue;
        }

        if (auto err = scan_device(bus, device))
        {
            return err;
        }
    }

    return Error::Code::SUCCESS;
}
}

namespace pci
{
void write_address(uint32_t address)
{
    IoOut32(kConfigAddress, address);
}

void write_data(uint32_t value)
{
    IoOut32(kConfigData, value);
}

uint32_t read_data()
{
    return IoIn32(kConfigData);
}

uint16_t read_vendor_id(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x00));
    return read_data() & 0xffffu;
}

uint16_t read_device_id(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x00));
    return (read_data() >> 16) & 0xffffu;
}

uint8_t read_header_type(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x0c));
    return (read_data() >> 16) & 0xffu;
}

uint32_t read_class_code(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x08));
    return read_data();
}

uint32_t read_bus_numbers(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x18));
    return read_data();
}

bool is_single_function_device(uint8_t header_type)
{
   return (header_type & 0x80u) == 0;
}

Error scan_all_bus()
{
    num_device = 0;

    auto header_type = read_header_type(0, 0, 0);
    if (is_single_function_device(header_type))
    {
        return scan_bus(0);
    }

    for (uint8_t function = 1; function < 8; function++)
    {
        if (read_vendor_id(0, 0, function) == 0xffffu)
        {
            continue;
        }

        if (auto err = scan_bus(function))
        {
            return err;
        }
    }

    return Error::Code::SUCCESS;
}
}
