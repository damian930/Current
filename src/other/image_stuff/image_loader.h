#ifndef EXTRA_IMAGE_LOADER_H
#define EXTRA_IMAGE_LOADER_H

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

#include "base/core.h"
#include "base/string.h"
#include "base/string.cpp" 

#include "base/arena.h"

struct Image_2d {
  U32 width;
  U32 height;
  U32 n_chanels;
  Data_buffer data_buffer_opt;
};

Image_2d load_png(Arena* arena, Str8 file_path, B32 do_flip_y)
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

  Image_2d image = {};
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