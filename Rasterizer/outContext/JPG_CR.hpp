

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


int write_JPEG_file(char const * filename, IMG_DATA const & img)
{
  
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  FILE * outfile;
  JSAMPROW row_pointer[1];
  int row_stride;

  cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = img.image_width; 	
  cinfo.image_height = img.image_height;
  cinfo.input_components = 3;		
  cinfo.in_color_space = JCS_RGB; 

  jpeg_set_defaults(&cinfo);

  jpeg_set_quality(&cinfo, img.quality, TRUE);

  jpeg_start_compress(&cinfo, TRUE);

  row_stride = img.image_width * 3;	

  while (cinfo.next_scanline < cinfo.image_height) {
 
    row_pointer[0] = & img.image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);

  fclose(outfile);

  jpeg_destroy_compress(&cinfo);

  return 0;
}