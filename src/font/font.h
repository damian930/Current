#ifndef D_FONT_H
#define D_FONT_H

#include "base/core.h"
#include "base/math.h"

// TODO: This shourd get removed from here later
#include "other/image_stuff/image_loader.h"

// TODO: Support codepoints like space that have width and height, but are not represented with a texture or a bb on atlas for plyph

struct Font_codepoint_data_node {
  Font_codepoint_data_node* next;
  Font_codepoint_data_node* prev;
  
  U8 codepoint;
  Bounding_box bb_on_atlas;
  Bounding_box glyph_bbox;

  // Scaled up front
  F32 advance_width;     
  F32 left_side_bearing; 
};

struct Font_hash_list {
  Font_codepoint_data_node* first;
  Font_codepoint_data_node* last;
  U32 node_count;
};

struct Font_kern_pair {
  U8 codepoint1;
  U8 codepoint2;
  F32 advance;
};

struct Font_kern_node {
  Font_kern_node* next;
  Font_kern_node* prev;
  Font_kern_pair kern_pair; // This is scaled up front
};

struct Font_kern_list {
  Font_kern_node* first;
  Font_kern_node* last;
  U32 count;
};

struct Font_info {
  // Font hash entries
  Font_hash_list hash_list[1]; // TODO: Make it not 1 

  Font_kern_list kern_list;

  // Font data (All these are scaled up front)
  Image2D font_atlas;
  U32 font_size;
  F32 scale;
  F32 ascent;
  F32 descent;
  F32 line_gap;
  F32 max_advance_width;
};

// Damian: Removed the ussual self contained layer from here, since not sure if need it yet
// void text_layer_init();
// void text_layer_release();

Font_info* load_font(Arena* arena, Range_U32 range_of_codepoints, 
                     U32 font_size, Str8 font_ttf_file_path);
Font_codepoint_data_node* font_get_codepoint_node_opt(Font_info* font_info, U8 codepoint);
Font_kern_pair* font_get_kern_pair_opt(Font_info* font_info, U8 codepoint1, U8 codepoint2);

// TODO: Rename this
// TODO: Rect with bb doesn work well, since there is a 1px error there for max in bb and x+width in rect
Rect codepoint_rect_from_font(Font_info* font_info, U8 codepoint);
// TODO: I really dislike that nulls are possible here
//       Maybe just have safe and unsafe versions ????
Rect codepoint_rect_from_data(Font_info* font_info, Font_codepoint_data_node* node);

Vec2_F32 font_measure_text(Font_info* font_info, Str8 text);

///////////////////////////////////////////////////////////
// Damian: Helpers
//
#include "render/render_gl_win32.h"

// DATE: 10 november 2025
// TODO: It would be nice to have a handle to the renderer to then just call its functions to load the texture,
//       but i couldnt figure out where to slam that handle, so no handle for now
Texture2D create_a_texture_from_font_atlas(Font_info* font_info);









#endif