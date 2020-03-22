#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cwchar>
#include <ios>
extern "C"
{
#include <fcntl.h>
#include <unistd.h>
}

namespace GR{

    struct Color
    {
        std::uint8_t b, g, r, a;
    };
    namespace Colors
    {
        Color const Black = {0, 0, 0, 255};
        Color const White = {255, 255, 255, 255};
        Color const Blue = {255, 0, 0, 255};
        Color const Green = {0, 255, 0, 255};
        Color const Red = {0, 0, 255, 255};
    }

    class Frame
    {
    protected:
        std::uint32_t const width_;
        std::uint32_t const height_;

         Frame()                                                      noexcept : Frame{1920u, 1080u} {}
         Frame(std::uint32_t const width, std::uint32_t const height) noexcept : width_(width), height_(height) {}
        ~Frame() {}
    };

    class Context : public Frame
    {
        using uint = std::uint32_t;
    
    protected:
        Color * const memory_;

    public:
         Context();
         Context(uint const width, uint const height);
        ~Context();

        Color *         operator[](uint const)       noexcept;
        Color const *   operator[](uint const) const noexcept;

        void Clear ()         noexcept;
        uint Size()     const noexcept;
    };

    class TTYContext final : public Context 
    {
    public:
         TTYContext();
        ~TTYContext() {}

        void Update() const;
    };
}