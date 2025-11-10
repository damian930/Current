#ifndef D_TEST_H
#define D_TEST_H

#include "base/string.h"
#include "other/image_stuff/image_loader.h"

#include "render/render_gl_win32.h"

// Damian: I have no idea how to work with font yet, so i am just doing stuff with it.
//         To now overthing and rewrite it often now, i just throw shit in the state and manage it.
//         When later i see how i want to manage it and how i want to use it, 
//         then i will know what i need and where i need it, then the rewrite will make sense 
struct Font_data {
  stbtt_fontinfo font_info;
  U32 font_size;

  U32 font_atlas_width;
  U32 font_atlas_height;
  Data_buffer font_atlas_data;

  B32 is_texture_present;
  Texture2D font_texture;
};

// TODO: Create a font atlas here
//       Reserve a gig of memory on it
//       This is for sure be enough for any real ttf file
struct Text_layer_state {
  Arena* perm_state_arena;
  Font_data* current_font_data;
};
global Text_layer_state* g_text_layer_state = {};

void text_layer_init();
void text_layer_release();

void text_load_font(const char* ttf_file_path, U32 font_size);
void text_unload_font();

// void text_load_font_onto_gpu();
// void text_unload_font_fron_gpu();

F32 text_get_text_dims(Str8 str);

///////////////////////////////////////////////////////////
// Damian: Helpers
//
Image2D create_bitmap_for_char_rgba(Arena* arena, stbtt_fontinfo* font_info, U8 codepoint, U32 font_size);






// Load font 
// Create the font data
// Store font data
// Create bitmaps for a font size







#endif