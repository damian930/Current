#ifndef D_TEST_H
#define D_TEST_H

#include "base/string.h"

#include "other/image_stuff/image_loader.h"
#include "base/math.h"

#include "base/string.cpp"
#include "base/math.cpp"

// #include "render/render_gl_win32.h"

#define STB_TRUETYPE_IMPLEMENTATION
#ifndef STB_TRUE_TYPE_H
#define STB_TRUE_TYPE_H
  #include "third_party/stb/stb_truetype.h"
#endif

struct Font_atlas_codepoint_offset {
  U32 x0;
  U32 y0;
  U32 x1;
  U32 y1;
};

// TODO: This need a better name for sure
struct Font_codepoint_data_node {
  Font_codepoint_data_node* next;
  Font_codepoint_data_node* prev;
  
  U8 codepoint;
  F32 advance_width;
  F32 left_side_bearing;
  Font_atlas_codepoint_offset codepoint_offset;
};

struct Font_hash_list {
  Font_codepoint_data_node* first;
  Font_codepoint_data_node* last;
  U32 node_count;
};

struct Font_info {
  // Font hash entries
  Font_hash_list hash_list[1];

  // Font data
  Image2D font_atlas;
  U32 font_size;
  F32 scale;
  F32 ascent;
  F32 descent;
  F32 line_gap;
};

// Damian: Removed the ussual self contained layer from here, since not sure if need it yet
// void text_layer_init();
// void text_layer_release();

Font_info* load_font(Arena* arena, Range_U32 range_of_codepoints, U32 font_size, Str8 font_ttf_file_path)
{
  Font_info* result_font_info = ArenaPush(arena, Font_info);

  Scratch scratch = get_scratch();
  {
    // Getting the ttf data, Creating the font
    Data_buffer ttf_data = read_file_inplace(scratch.arena, font_ttf_file_path);

    stbtt_fontinfo font_info = {};
    stbtt_InitFont(&font_info, ttf_data.data, stbtt_GetFontOffsetForIndex(ttf_data.data, 0));
  
    // Making sure that the font can be used 
    B32 are_all_codepoints_supported = true;
    ForEachRangeU32(range_index, range_of_codepoints)
    {
      if (stbtt_FindGlyphIndex(&font_info, range_index) == 0) {
        are_all_codepoints_supported = false;
        break;
      }
    }
    Assert(are_all_codepoints_supported);

    // Creating the font atlas
    U32 count_of_codepints = range_u32_count(range_of_codepoints);
    stbtt_packedchar* glyph_metrics = ArenaPushArr(scratch.arena, stbtt_packedchar, count_of_codepints);
    stbtt_pack_range ranges = {(F32)font_size, 
                                (int)range_of_codepoints.min, 
                                NULL, 
                                (int)count_of_codepints, 
                                glyph_metrics, 
                                Null, Null};
    Image2D font_atlas = {};
    font_atlas.data_buffer = data_buffer_make(arena, 1024 * 1024);
    font_atlas.width       = 1024;
    font_atlas.height      = 1024;
    font_atlas.n_chanels   = 1;

    // TODO: Would be nice to be able to predict the size for the atlas
    //       insted of just manually guessing the size
    stbtt_pack_context pc = {};
    int is_succ = stbtt_PackBegin(&pc, font_atlas.data_buffer.data, 
                                  font_atlas.width, 
                                  font_atlas.height, 
                                  0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1); 
    stbtt_PackFontRanges(&pc, ttf_data.data, 0, &ranges, 1);
    stbtt_PackEnd(&pc);
    Assert(is_succ);
    
    // Storing the data for the font 
    result_font_info->font_atlas = font_atlas;
    result_font_info->font_size = font_size;

    F32 scale = stbtt_ScaleForPixelHeight(&font_info, font_size);
    result_font_info->scale = scale; 
    {
      int ascent  = {};
      int descent = {};
      int linegap = {};
      stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &linegap);
      result_font_info->ascent  = (F32)ascent  * scale;
      result_font_info->descent = (F32)descent * scale;
      result_font_info->line_gap = (F32)linegap * scale;
    }

    // Codepoint specific data
    ForEachRangeU32(it_codepoint, range_of_codepoints)
    {
      int advance_width = {};
      int left_side_bearing = {};
      stbtt_GetCodepointHMetrics(&font_info, (int)it_codepoint, &advance_width, &left_side_bearing);

      // TODO: This has to be changed
      //       Left it this way, since we only have 100 characters for noe
      // NOTE: What a great hash function
      U32 codepoint_hash = 0; 
      Font_hash_list* list = result_font_info->hash_list + codepoint_hash;
      
      U32 codepoint_pached_data_index = it_codepoint - range_of_codepoints.min;
      stbtt_packedchar* codepoint_pached_data = glyph_metrics + codepoint_pached_data_index;
  
      Font_codepoint_data_node* new_node = ArenaPush(arena, Font_codepoint_data_node);
      new_node->codepoint         = it_codepoint;
      new_node->advance_width     = advance_width;
      new_node->left_side_bearing = left_side_bearing;
      new_node->codepoint_offset  = Font_atlas_codepoint_offset{codepoint_pached_data->x0, 
                                                                codepoint_pached_data->y0, 
                                                                codepoint_pached_data->x1, 
                                                                codepoint_pached_data->y1};
      DllPushBack(list, new_node);
      list->node_count += 1;
    }

    // // TODO: See if OS/2 ascent,descent... are needed
    // int advance_width = {};
    // int left_side_bearing = {};
    // stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance_width, &left_side_bearing);
  }
  end_scratch(&scratch);

  return result_font_info; 
}

Font_codepoint_data_node* font_get_codepoint_node_opt(Font_info* font_info, U8 codepoint) {
  Font_codepoint_data_node* result = 0;
  for (Font_codepoint_data_node* node = font_info->hash_list->first; 
       node != 0; 
       node = node->next
  ) {
    if (node->codepoint == codepoint)
    {
      result = node;
      break;
    }
  }
  return result;
};

F32 crop_codepoint_from_font_atlas_texture(Font_info* font_info, U8 codepoint)
{
  Font_codepoint_data_node* node = font_get_codepoint_node_opt(font_info, codepoint);
  
  
  
  node->codepoint_offset;

  NotImplemented();

  return 0;
}

///////////////////////////////////////////////////////////
// Damian: Helpers
//
#include "render/render_gl_win32.h"

Texture2D create_a_texture_from_font_atlas(Font_info* font_info)
{
  // DATE: 10 november 2025
  // TODO: It would be nice to have a handle to the renderer to then just call its functions to load the texture,
  //       but i couldnt figure out where to slam that handle, so no handle for now

  Texture2D texture = {};
  Scratch scratch = get_scratch();
  {
    U32 h = font_info->font_atlas.width;
    U32 w = font_info->font_atlas.height;
    
    U64 input_data_size = w * h * 1; // Damian: stb only uses 1 byte per pixel in "stbtt_GetCodepointBitmap"
    U64 my_data_size    = w * h * 4; // Damian: RGBA
    Data_buffer my_data = data_buffer_make(scratch.arena, my_data_size);
    
    U8* image_byte    = font_info->font_atlas.data_buffer.data;
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
    Image2D bitmap = {}; 
    bitmap.width           = w;
    bitmap.height          = h;
    bitmap.n_chanels       = 4;
    bitmap.data_buffer_opt = my_data;
    
    texture = load_texture(bitmap);
  }
  end_scratch(&scratch);
  return texture;
}





// Load font 
// Create the font data
// Store font data
// Create bitmaps for a font size







#endif