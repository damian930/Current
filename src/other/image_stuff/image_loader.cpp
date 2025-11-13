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
    Str8 path_nt = str8_from_str8(scratch.arena, file_path);
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





#endif