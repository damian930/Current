#ifndef EXTRA_IMAGE_LOADER_CPP
#define EXTRA_IMAGE_LOADER_CPP

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION  
#include "third_party/stb/stb_truetype.h"

#include "image_loader.h"
#include "base/string.cpp"
#include "base/arena.cpp"

Image2D load_png(Arena* arena, Str8 file_path, B32 do_flip_y)
{ 
  Str8 ext = get_file_extension(file_path);
  if (!str8_match_cstr(ext, "png", Str8_match_flag_NONE))
  {
    printf("NO MATCH PNG \n");
    exit(1);
  }

  unsigned char* data = 0;
  int width = 0, height = 0, nrChannels = 0;
  Scratch scratch = get_scratch();
  {
    stbi_set_flip_vertically_on_load(do_flip_y);
    Str8 path_nt = str8_from_str8_temp_null_term(scratch.arena, file_path);
    data = stbi_load("../data/jimmy.png", &width, &height, &nrChannels, 0);
    int x = 0;
  }
  end_scratch(&scratch);

  Image2D image = {};
  if (data)
  {
    image.width = (U32)width;
    image.height = (U32)height;
    image.n_chanels = (U32)nrChannels;

    U64 data_size = image.width * image.height * image.n_chanels; 
    image.data_buffer_opt = data_buffer_make(arena, data_size);
    MemCopy(image.data_buffer_opt.data, data, data_size);
  }
  else 
  {
    const char* error_message = stbi_failure_reason();
    fprintf(stderr, "Error loading image: %s\n", error_message);
  }
  stbi_image_free(data);

  return image;
}

Image2D create_bitmap_for_char_rgba(Arena* arena, stbtt_fontinfo* font_info, U8 codepoint, U32 font_size)
{
  Image2D bitmap = {}; 
  {
    int w    = {};  
    int h    = {};  
    int xoff = {};  
    int yoff = {};  
    U8* stb_data = stbtt_GetCodepointBitmap(font_info, 0, stbtt_ScaleForPixelHeight(font_info, (F32)font_size), 
                                            codepoint, &w, &h, &xoff, &yoff); 

    U64 stb_data_size = w * h * 1; // Damian: stb only uses 1 byte per pixel in "stbtt_GetCodepointBitmap"
    U64 my_data_size  = w * h * 4; // Damian: RGBA
    Data_buffer my_data = data_buffer_make(arena, my_data_size);

    U8* image_byte    = stb_data;
    U32* bitmap_pixel = (U32*)my_data.data;

    for (U32 image_y = 0; image_y < h; image_y += 1)
    {
      for (U32 image_x = 0; image_x < w; image_x += 1)
      {
        U32 byte = (U32)*image_byte;
        image_byte += 1;
        
        *bitmap_pixel = ((byte << 24) |
                        (byte << 16) |
                        (byte << 8)  |
                        (byte << 0)  );
        bitmap_pixel += 1;
      }
    }
    stbtt_FreeBitmap(stb_data, (void*)0); 
    // TODO: All this malloc/free stuff might be removed if i suply my own arena alloc/release to stb 
    
    bitmap.width           = w;
    bitmap.height          = h;
    bitmap.n_chanels       = 4;
    bitmap.data_buffer_opt = my_data;
  }
  return bitmap;
}



#endif