#include <cstdint>

extern "C" void KernelMain(
    uint64_t framebuffer_addr,
    uint64_t framebuffer_size
)
{
    auto framebuffer = reinterpret_cast<uint8_t*>(framebuffer_addr);
    for (uint64_t i = 0; i < framebuffer_size; ++i)
    {
        framebuffer[i] = i % 256;
    }

    while (true) __asm__("hlt");
}
