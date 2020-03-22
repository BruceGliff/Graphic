 

//void Dump(TTYContext const & context)
// {
//     static int frame = 0;
//     int const w = context.width;
//     int const h = context.height;

//     IMG_DATA img(w, h, 100);
//     for(int i = 0; i < h; ++i)
//         for(int j = 0; j < w; ++j)
//         {
//             color const * const ptr = context.memory + w * i + j;
            
//             JSAMPLE * p = img.image_buffer + (w * i + j) * 3;
//             *p = static_cast<JSAMPLE>(ptr->r);
//             *(p+1) = static_cast<JSAMPLE>(ptr->g);
//             *(p+2) = static_cast<JSAMPLE>(ptr->b);           
//         }

//     std::ostringstream out_path_stream;
//     out_path_stream << "direct_tr/" << frame << ".jpeg";

//     write_JPEG_file(out_path_stream.str().c_str(), img);
//     ++frame;
// }