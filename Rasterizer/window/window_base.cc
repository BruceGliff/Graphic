#include "window_base.hpp"

using namespace GR;

Context::Context() :
    Context{1920u, 1080u}
{}
Context::Context(uint const width, uint const height) :
    Frame{width, height},
    memory_(static_cast<Color *>(std::calloc(width_ * height_, sizeof(Color))))
{

}
Context::~Context()
{
    std::free(memory_);
}
Color * Context::operator[](uint const y) noexcept
{
    //return memory_ + width_ * (height_ - y - 1);
    return memory_ + width_ * y;
}
Color const * Context::operator[](uint const y) const noexcept
{
    //return memory_ + width_ * (height_ - y - 1);
    return memory_ + width_ * y;
}
void Context::Clear() noexcept
{
    std::wmemset(reinterpret_cast<wchar_t *>(memory_), 0xff000000, width_ * height_);
}
uint Context::Size() const noexcept
{
    return width_ * height_ * sizeof(Color);
}


TTYContext::TTYContext()
{
    int fb0 = ::open("/dev/fb0", O_WRONLY);
    if (!fb0)
        throw std::ios_base::failure{"Can not open fb0"};

    ::close(fb0);
}
void TTYContext::Update() const
{
    int fb0 = ::open("/dev/fb0", O_WRONLY);
    if (!fb0)
        throw std::ios_base::failure{"Update Error: Can not open fb0"};

    ::write(fb0, memory_, Size());
    ::close(fb0);
}