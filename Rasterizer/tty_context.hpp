#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cwchar>
extern "C"
{
#include <fcntl.h>
#include <unistd.h>
}
struct TTYContext
{
    struct color
    {
        std::uint8_t b, g, r, a;
    };
    using uint = std::uint32_t;

    uint    const width;
    uint    const height;
    color * const memory;

    ~TTYContext();
     TTYContext() noexcept;

    color *         operator[](uint const)       noexcept;
    color const *   operator[](uint const) const noexcept;

    void clear () noexcept;
    void update() noexcept;
};

inline TTYContext::TTYContext() noexcept
    : width(1920u)
    , height(1080u)
    , memory(static_cast<color *>(std::calloc(width * height, sizeof(color))))
{}
inline TTYContext::~TTYContext()
{
    std::free(memory);
}
inline TTYContext::color *TTYContext::operator[](uint const y) noexcept
{
    return memory + width * (height - y - 1);
}
inline TTYContext::color const *TTYContext::operator[](uint const y) const noexcept
{
    return memory + width * (height - y - 1);
}
inline void TTYContext::clear() noexcept
{
    std::wmemset(reinterpret_cast<wchar_t *>(memory), 0xff000000, width * height);
}
inline void TTYContext::update() noexcept
{
    int fb0 = ::open("/dev/fb0", O_WRONLY);
    assert(fb0 != -1);
    ::write(fb0, memory, width * height * sizeof(color));
    ::close(fb0);
}
