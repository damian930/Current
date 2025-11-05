#ifndef EXTRA_IMAGE_LOADER_H
#define EXTRA_IMAGE_LOADER_H

#include "base/core.h"
#include "base/string.h"
#include "base/arena.h"
#include "os/core/os_core_win32.h"

#include "third_party/stb/stb_truetype.h"

struct Image2D {
  U32 width;
  U32 height;
  U32 n_chanels;
  Data_buffer data_buffer_opt;
};

Image2D load_png(Arena* arena, Str8 file_path, B32 do_flip_y);
Image2D create_bitmap_for_char_rgba(Arena* arena, stbtt_fontinfo* font_info, U8 codepoint, U32 font_size);




#if 0
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  // force following include to generate implementation
#include "stb_truetype.h"

char ttf_buffer[1<<25];

int main(int argc, char **argv)
{
   stbtt_fontinfo font;
   unsigned char *bitmap;
   int w,h,i,j,c = (argc > 1 ? atoi(argv[1]) : 'a'), s = (argc > 2 ? atoi(argv[2]) : 20);

   fread(ttf_buffer, 1, 1<<25, fopen(argc > 3 ? argv[3] : "c:/windows/fonts/arialbd.ttf", "rb"));

   stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer,0));
   bitmap = stbtt_GetCodepointBitmap(&font, 0,stbtt_ScaleForPixelHeight(&font, s), c, &w, &h, 0,0);

   for (j=0; j < h; ++j) {
      for (i=0; i < w; ++i)
         putchar(" .:ioVM@"[bitmap[j*w+i]>>5]);
      putchar('\n');
   }
   return 0;
}
#endif

// Texture load_texture(const char* path)
// {
//   Texture result = {};
//   glGenTextures(1, &result.id);
//   glBindTexture(GL_TEXTURE_2D, result.id); 

//   // TODO: see what these do exactly (copied them from the book)
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//   stbi_set_flip_vertically_on_load(true);

//   int width, height, nrChannels;
//   unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
//   if (data)
//   {
//     Assert(width >= 0 && height >= 0 && nrChannels >= 0)
//     result.width = (U32)width;
//     result.height = (U32)height;
//     result.n_chanels = (U32)nrChannels;
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//     glGenerateMipmap(GL_TEXTURE_2D);
//   }
//   else
//   {
//     core_push_err_c("Failed to load texture");
//     exit(1);
//   }
//   stbi_image_free(data);
  
//   // Load it into the gpu mem permanently
//   return result;







#endif