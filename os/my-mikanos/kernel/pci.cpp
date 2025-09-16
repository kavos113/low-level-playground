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

Error add_device(const Device& device)
{
    if (num_device == devices.size())
    {
        return Error::Code::FULL;
    }

    devices[num_device] = device;
    num_device++;
    return Error::Code::SUCCESS;
}

Error scan_bus(uint8_t bus);

Error scan_function(uint8_t bus, uint8_t device, uint8_t function)
{
    auto class_code = read_class_code(bus, device, function);
    auto header_type = read_header_type(bus, device, function);
    Device dev{bus, device, function, header_type, class_code};
    if (auto err = add_device(dev))
    {
        return err;
    }

    if (class_code.match(0x06u, 0x04u))
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

ClassCode read_class_code(uint8_t bus, uint8_t device, uint8_t function)
{
    write_address(make_config_address(bus, device, function, 0x08));
    auto reg = read_data();
    ClassCode cc = {};
    cc.base = (reg >> 24) & 0xffu;
    cc.sub = (reg >> 16) & 0xffu;
    cc.interface = (reg >> 8) & 0xffu;
    return cc;
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

uint32_t read_config_register(const Device& dev, uint8_t reg_addr)
{
    write_address(make_config_address(dev.bus, dev.device, dev.function, reg_addr));
    return read_data();
}

void write_config_register(const Device& dev, uint8_t reg_addr, uint32_t value)
{
    write_address(make_config_address(dev.bus, dev.device, dev.function, reg_addr));
    write_data(value);
}

uint8_t calc_bar_address(unsigned int bar_index)
{
    return 0x10 + 4 * bar_index;
}

Error read_bar(Device& device, unsigned int bar_index, uint64_t* out)
{
    if (bar_index >= 6)
    {
        return Error::Code::INDEX_OUT_OF_RANGE;
    }

    const auto addr = calc_bar_address(bar_index);
    const auto bar = read_config_register(device, addr);

    if ((bar & 4u) == 0)
    {
        *out = bar;
        return Error::Code::SUCCESS;
    }

    if (bar_index >= 5)
    {
        *out = 0;
        return Error::Code::INDEX_OUT_OF_RANGE;
    }

    const auto bar_upper = read_config_register(device, addr + 4);
    *out = bar | (static_cast<uint64_t>(bar_upper) << 32);
    return Error::Code::SUCCESS;
}
}
