#pragma once
#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cwchar>
#include <ios>
#include <fstream>
#include <sstream>
#include "../outContext/JPG_CR.hpp"
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
    public:
        std::uint32_t const width_;
        std::uint32_t const height_;
    protected:
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

        void Dump()
        {
            int const w =  static_cast<int>(width_);
            int const h = static_cast<int>(height_);


            std::ofstream out("out.ppm");
            out << "P6\n" << w << " " << h << "\n255\n";
            for(int i = 0; i < h; ++i)
                for(int j = 0; j < w; ++j)
                {
                    Color const * const ptr = memory_ + w * i + j;
                    Color const c = {ptr->r, ptr->g, ptr->b, 255};
                    out.write(reinterpret_cast<char const *>(&c), 3);
                } 
        }


        void DumpJPEG()
        {
            static int frame = 0;
            int const w =  static_cast<int>(width_);
            int const h = static_cast<int>(height_);

            IMG_DATA img(w, h, 100);
            for(int i = 0; i < h; ++i)
                for(int j = 0; j < w; ++j)
                {
                    Color const * const ptr = memory_ + w * i + j;

                    JSAMPLE * p = img.image_buffer + (w * i + j) * 3;
                    *p = static_cast<JSAMPLE>(ptr->r);
                    *(p+1) = static_cast<JSAMPLE>(ptr->g);
                    *(p+2) = static_cast<JSAMPLE>(ptr->b);           
                }

            std::ostringstream out_path_stream;
            out_path_stream << "direct_tr/" << frame << ".jpeg";

            write_JPEG_file(out_path_stream.str().c_str(), img);
            ++frame;
        }

        void Update() const;
    };
}