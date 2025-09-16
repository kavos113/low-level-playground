#include "console.h"
#include "frame_buffer_config.h"
#include "logger.h"
#include "mouse.h"
#include "pci.h"
#include "pixel_writer.h"

#include "usb/classdriver/mouse.hpp"
#include "usb/xhci/xhci.hpp"

void operator delete(void* obj) noexcept
{
}

constexpr PixelColor kWindowBgColor = {69, 92, 204};
constexpr PixelColor kWindowFgColor = {245, 245, 245};

char pixel_writer_buf[sizeof(RGBPixelWriter)];
PixelWriter* pixel_writer;

char console_buf[sizeof(Console)];
Console* console;

char mouse_cursor_buf[sizeof(MouseCursor)];
MouseCursor* mouse_cursor;

void mouse_observer(int8_t displacement_x, int8_t displacement_y)
{
    Log(LogLevel::DEBUG, "mouse move: (%d, %d)\n", displacement_x, displacement_y);
    mouse_cursor->move_relative({displacement_x, displacement_y});
}

void switch_ehci_to_xhci(const pci::Device& xhc_dev)
{
    bool intel_exc_exist = false;

    for (int i = 0; i < pci::num_device; ++i)
    {
        if (pci::devices[i].class_code.match(0x0cu, 0x03u, 0x20u) && pci::read_vendor_id(pci::devices[i]) == 0x8086)
        {
            intel_exc_exist = true;
            break;
        }
    }

    if (!intel_exc_exist)
    {
        return;
    }

    uint32_t superspeed_ports = pci::read_config_register(xhc_dev, 0xdc);
    pci::write_config_register(xhc_dev, 0xd8, superspeed_ports);

    uint32_t echi2xhci_ports = pci::read_config_register(xhc_dev, 0xd4);
    pci::write_config_register(xhc_dev, 0xd0, echi2xhci_ports);
}

extern "C" void KernelMain(const FrameBufferConfig* config)
{
    switch (config->pixel_format)
    {
    case kPixelRGBResv8BitPerColor:
        pixel_writer = new(pixel_writer_buf) RGBPixelWriter(config);
        break;

    case kPixelBGRResv8BitPerColor:
        pixel_writer = new(pixel_writer_buf) BGRPixelWriter(config);
    }

    int frame_width = static_cast<int>(config->horizontal_resolution);
    int frame_height = static_cast<int>(config->vertical_resolution);

    console = new(console_buf) Console(pixel_writer, kWindowFgColor, kWindowBgColor);

    fill_rect(*pixel_writer, {0, 0}, {frame_width, frame_height}, kWindowBgColor);
    fill_rect(*pixel_writer, {0, frame_height - 50}, {frame_width, 50}, kWindowFgColor);
    fill_rect(*pixel_writer, {0, frame_height - 50}, {frame_width / 5, 50}, {155, 155, 155});

    SetLogLevel(LogLevel::DEBUG);
    Log(LogLevel::DEBUG, "Hello, OS!\n");

    mouse_cursor = new(mouse_cursor_buf) MouseCursor{pixel_writer, kWindowBgColor, {200, 200}};

    auto err = pci::scan_all_bus();
    Log(LogLevel::DEBUG, "scan_all_bus is finished with: %s\n", err.name());

    for (int i = 0; i < pci::num_device; ++i)
    {
        const auto& device = pci::devices[i];

        auto vendor_id = pci::read_vendor_id(device.bus, device.device, device.function);
        auto class_code = pci::read_class_code(device.bus, device.device, device.function);

        Log(
            LogLevel::DEBUG, "%d.%d.%d: vendor %04x, class %08x, head %02x\n",
            device.bus, device.device, device.function, vendor_id, class_code, device.header_type
        );
    }

    SetLogLevel(LogLevel::WARN);

    pci::Device* xhc_dev = nullptr;
    for (int i = 0; i < pci::num_device; ++i)
    {
        if (pci::devices[i].class_code.match(0x0cu, 0x03u, 0x30u)) // serial bus / usb / xHCI
        {
            xhc_dev = &pci::devices[i];

            if (pci::read_vendor_id(*xhc_dev) == 0x8086) // intel
            {
                break;
            }
        }
    }

    if (xhc_dev)
    {
        Log(LogLevel::WARN, "xHC has been found: %d.%d.%d\n", xhc_dev->bus, xhc_dev->device, xhc_dev->function);
    }

    uint64_t xhc_bar;
    err = pci::read_bar(*xhc_dev, 0, &xhc_bar);
    if (err)
    {
        Log(LogLevel::ERROR, "failed to read xHC BAR: %s\n", err.name());
    }

    uint64_t xhc_mmio_base = xhc_bar & ~static_cast<uint64_t>(0xf);
    Log(LogLevel::WARN, "xHC mmio_base = %08lx\n", xhc_mmio_base);

    usb::xhci::Controller xhc{xhc_mmio_base};

    if (pci::read_vendor_id(*xhc_dev) == 0x8086)
    {
        switch_ehci_to_xhci(*xhc_dev);
    }

    err = xhc.Initialize();
    if (err)
    {
        Log(LogLevel::ERROR, "failed to initialize xHC: %s\n", err.name());
    }

    xhc.Run();

    usb::HIDMouseDriver::default_observer = mouse_observer;

    for (int i = 1; i <= xhc.MaxPorts(); ++i)
    {
        auto port = xhc.PortAt(i);

        if (port.IsConnected())
        {
            if ((err = usb::xhci::ConfigurePort(xhc, port)))
            {
                Log(LogLevel::ERROR, "failed to configure port: %s\n", err.name());
                continue;
            }
        }
    }

    while (true)
    {
        err = usb::xhci::ProcessEvent(xhc);
        if (err)
        {
            Log(LogLevel::ERROR, "error while process event: %s, %d at %s\n", err.name(), err.line(), err.file());
        }
    }

    while (true)
    {
        __asm__("hlt");
    }
}
