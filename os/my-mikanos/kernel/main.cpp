#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdarg>

#include "console.h"
#include "font.h"
#include "frame_buffer_config.h"
#include "pci.h"
#include "pixel_writer.h"

void operator delete(void* obj) noexcept
{
}

constexpr PixelColor kWindowBgColor = {69, 92, 204};
constexpr PixelColor kWindowFgColor = {245, 245, 245};

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

char pixel_writer_buf[sizeof(RGBPixelWriter)];
PixelWriter *pixel_writer;

char console_buf[sizeof(Console)];
Console *console;

int printk(const char *format, ...)
{
    va_list ap;
    char s[1024];

    va_start(ap, format);
    int result = vsprintf(s, format, ap);
    va_end(ap);

    console->put_string(s);
    return result;
}

extern "C" void KernelMain(const FrameBufferConfig *config)
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

    printk("Hello, OS!");

    for (int y = 0; y < kMouseCursorHeight; ++y)
    {
        for (int x = 0; x < kMouseCursorWidth; ++x)
        {
            if (mouse_cursor_shape[y][x] == '@')
            {
                pixel_writer->write(200 + x, 200 + y, {0, 0, 0});
            }
            else if (mouse_cursor_shape[y][x] == '.')
            {
                pixel_writer->write(200 + x, 200 + y, {255, 255, 255});
            }
        }
    }

    auto err = pci::scan_all_bus();
    printk("scan_all_bus is finished with: %s\n", err.name());

    for (int i = 0; i < pci::num_device; ++i)
    {
        const auto& device = pci::devices[i];

        auto vendor_id = pci::read_vendor_id(device.bus, device.device, device.function);
        auto class_code = pci::read_class_code(device.bus, device.device, device.function);

        printk("%d.%d.%d: vendor %04x, class %08x, head %02x\n",
            device.bus, device.device, device.function, vendor_id, class_code, device.header_type);
    }

    pci::Device *xhc_dev = nullptr;
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
        printk("xHC has been found: %d.%d.%d\n", xhc_dev->bus, xhc_dev->device, xhc_dev->function);
    }

    uint64_t xhc_bar;
    err = pci::read_bar(*xhc_dev, 0, &xhc_bar);
    if (err)
    {
        printk("read_bar: %s\n", err.name());
    }

    uint64_t xhc_mmio_base = xhc_bar & ~static_cast<uint64_t>(0xf);
    printk("xHC mmio_base = %08lx\n", xhc_mmio_base);

    while (true)
    {
        __asm__("hlt");
    }
}