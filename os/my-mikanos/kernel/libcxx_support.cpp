#include <new>
#include <cerrno>

std::new_handler std::get_new_handler() noexcept
{
    return nullptr;
}

extern "C" int posix_memalign(void**, size_t, size_t)
{
    return ENOMEM;
}

extern "C" void __cxa_pure_virtual()
{
    while (1) __asm__("hlt");
}
