#pragma once
#include <cassert>
extern "C"
{
#include <fcntl.h>
#include <unistd.h>
}

struct Mouse
{
    struct event
    {
        char flags;
        char dx;
        char dy;

        bool   left_button() const noexcept {return (flags & 0x1) != 0;}
        bool  right_button() const noexcept {return (flags & 0x2) != 0;}
        bool middle_button() const noexcept {return (flags & 0x4) != 0;}
    };

    ~Mouse();
    Mouse() noexcept;

    bool poll(event &e) noexcept;

private:
    int const mouse_fd;
};

inline Mouse::Mouse() noexcept
    : mouse_fd(::open("/dev/input/mice", O_RDONLY | O_NONBLOCK))
{
    assert(mouse_fd != -1);
}
inline Mouse::~Mouse()
{
    ::close(mouse_fd);
}
inline bool Mouse::poll(event &e) noexcept
{
    return ::read(mouse_fd, &e, sizeof(event)) > 0;
}
