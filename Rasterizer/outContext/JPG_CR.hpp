#pragma once

#include <cstdlib>
#include <cstdio>

extern "C"
{
    #include <jpeglib.h>
}


struct IMG_DATA
{
    int image_width;
    int image_height;
    JSAMPLE * image_buffer;

    int quality;

    IMG_DATA(int w, int h, int q) :
      image_width(w),
      image_height(h),
      quality(q)
    {
        image_buffer = new JSAMPLE[w * h * 3];
    }

    ~IMG_DATA()
    {
      delete image_buffer;
    }
};


int write_JPEG_file(char const * filename, IMG_DATA const & img);