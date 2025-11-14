#ifndef D_FONT_CPP
#define D_FONT_CPP

#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
  #include "third_party/stb/stb_image_write.h"
#endif

// #include "other/image_stuff/image_loader.cpp"

#define STB_TRUETYPE_IMPLEMENTATION

#include "font.h"
#include "base/math.cpp"

Font_info* load_font(Arena* arena, Range_U32 range_of_codepoints, 
                     U32 font_size, Str8 font_ttf_file_path
) {
  Font_info* result_font_info = ArenaPush(arena, Font_info);

  Scratch scratch = get_scratch();
  {
    // Getting the ttf data, Creating the font
    Data_buffer ttf_data = os_win32_file_read_inplace(scratch.arena, font_ttf_file_path);

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

    #if DEBUG_MODE 
    stbi_write_png("font_atlas_test.png", font_atlas.width, font_atlas.height, 1, font_atlas.data_buffer.data, 0);
    #endif
    // Storing the data for the font 
    result_font_info->font_atlas = font_atlas;
    result_font_info->font_size = font_size;

    F32 scale = stbtt_ScaleForPixelHeight(&font_info, (F32)font_size);
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
      F32 advance_width = {};
      F32 left_side_bearing = {};
      {
        int int_advance_width = {};
        int int_left_side_bearing = {};
        stbtt_GetCodepointHMetrics(&font_info, (int)it_codepoint, &int_advance_width, &int_left_side_bearing);
        advance_width = scale * (F32)int_advance_width;
        left_side_bearing = scale * (F32)left_side_bearing;
      }
      ValueToMax(result_font_info->max_advance_width, advance_width);

      Bounding_box glyph_bbox = {};
      {
        // TODO: Make sure that this funstion return x1 and y1 non inclusive style.
        int x0 = {};
        int y0 = {};
        int x1 = {};
        int y1 = {};
        int succ = stbtt_GetCodepointBox(&font_info, it_codepoint, &x0, &y0, &x1, &y1);
        Assert(succ);
        glyph_bbox.x0 = scale * (F32)x0;
        glyph_bbox.y0 = scale * (F32)y0;
        glyph_bbox.x1 = scale * (F32)x1;
        glyph_bbox.y1 = scale * (F32)y1;
      }

      // TODO: This has to be changed
      //       Left it this way, since we only have 100 characters for noe
      // NOTE: What a great hash function
      U32 codepoint_hash = 0; 
      Font_hash_list* list = result_font_info->hash_list + codepoint_hash;
      
      U32 codepoint_pached_data_index = it_codepoint - range_of_codepoints.min;
      stbtt_packedchar* codepoint_pached_data = glyph_metrics + codepoint_pached_data_index;
  
      // TODO: I here use asci only, but we need to split U32 codepoint unicode id into separat U8 parts if we are using them
      //       Leaving the compiler time warning for this no not forget.
      Font_codepoint_data_node* new_node = ArenaPush(arena, Font_codepoint_data_node);
      new_node->codepoint         = it_codepoint;
      new_node->advance_width     = advance_width;
      new_node->left_side_bearing = left_side_bearing;
      // TODO: Make sure that this funstion return x1 and y1 non inclusive style.
      new_node->bb_on_atlas = bounding_box_make(codepoint_pached_data->x0, 
                                                codepoint_pached_data->y0, 
                                                codepoint_pached_data->x1, 
                                                codepoint_pached_data->y1);
      new_node->glyph_bbox = glyph_bbox;
      
      DllPushBack(list, new_node);
      list->node_count += 1;
    }

    // Kerning table stuff 
    {
      int kern_entries_count = stbtt_GetKerningTableLength(&font_info);
      stbtt_kerningentry* kern_entries = ArenaPushArr(scratch.arena, stbtt_kerningentry, kern_entries_count);
      int entries_returned = stbtt_GetKerningTable(&font_info, kern_entries, kern_entries_count);
      Assert(entries_returned == kern_entries_count);
      
      Font_kern_list* kern_list = &result_font_info->kern_list;
      ForEachEx(kern_index, kern_entries_count, kern_entries)
      {
        stbtt_kerningentry* entry = kern_entries + kern_index;

        if (   range_u32_within(range_of_codepoints, entry->glyph1)
            && range_u32_within(range_of_codepoints, entry->glyph2)
        ) {
          Font_kern_node* new_node = ArenaPush(arena, Font_kern_node);
          new_node->kern_pair.codepoint1 = entry->glyph1;
          new_node->kern_pair.codepoint2 = entry->glyph2;
          new_node->kern_pair.advance = scale * (F32)entry->advance;
  
          DllPushBack(kern_list, new_node);
          kern_list->count += 1;
        }
      }
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

Font_kern_pair* font_get_kern_pair_opt(Font_info* font_info, U8 codepoint1, U8 codepoint2)
{
  Font_kern_pair* result = 0;
  for (Font_kern_node* node = font_info->kern_list.first; node != 0; node = node->next)
  {
    if (   node->kern_pair.codepoint1 == codepoint1 
        && node->kern_pair.codepoint2 == codepoint2
    ) {
      result = &node->kern_pair;
      break;
    }
  }
  return result;
}

Rect codepoint_rect_from_font(Font_info* font_info, U8 codepoint)
{
  Font_codepoint_data_node* node = font_get_codepoint_node_opt(font_info, codepoint);
  Rect source_rect = {}; // Damian: Default source_rect in case codepoint is not supported
  source_rect.x = 0.0f;
  source_rect.y = 0.0f;
  source_rect.width = font_info->max_advance_width;  
  source_rect.height = font_info->ascent + Abs(F32, font_info->descent);
  if (node) {
    source_rect = rect_from_bbox(node->bb_on_atlas);
  }
  return source_rect;
}

// TODO: I really dislike that nulls are possible here
//       Maybe just have safe and unsafe versions ????
#if 1 // TODO: Remove this if it doesnt get used ever (I commented it out on 14th of December)
Rect codepoint_rect_from_data(Font_info* font_info, Font_codepoint_data_node* node)
{
  Rect source_rect = {};
  if (node) 
  {
    source_rect.x = node->bb_on_atlas.x0;
    source_rect.y = node->bb_on_atlas.y0;
    source_rect.width = node->bb_on_atlas.x1 - node->bb_on_atlas.x0;
    source_rect.height = node->bb_on_atlas.y1 - node->bb_on_atlas.y0;
  }
  else 
  {
    source_rect.x = 0.0f;
    source_rect.y = 0.0f;
    source_rect.width = font_info->max_advance_width;
    source_rect.height = font_info->ascent + Abs(F32, font_info->descent);
  }
  return source_rect;
}
#endif

Vec2_F32 font_measure_text(Font_info* font_info, Str8 text)
{
  F32 result_width = 0.0f;
  F32 result_height = font_info->ascent + Abs(F32, font_info->descent);

  F32 top_line_y = font_info->ascent;
  ForEachEx(index, text.count, text.data)
  {
    U8 codepoint = text.data[index];
    
    Font_codepoint_data_node* data = font_get_codepoint_node_opt(font_info, codepoint);
    if (data)
    {
      // result_width += data->left_side_bearing;
      result_width += data->advance_width;
      if (index < text.count - 1)
      {
        U8 next_codepoint = text.data[index + 1];
        Font_kern_pair* kern_pair = font_get_kern_pair_opt(font_info, codepoint, next_codepoint);      
        if (kern_pair)
        {
          result_width += kern_pair->advance;
        }
      }
    }
    else 
    {
      result_width += font_info->max_advance_width;
    }
  } 

  Vec2_F32 result_dims = vec2_f32(result_width, result_height);
  return result_dims;

  // TODO: Add a new line handling here
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
        
        *bitmap_pixel = ((byte << 24) | // Alpha
        (byte << 16) | // Blue
        (byte << 8)  | // Green
        (byte << 0)  ); // Red
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

// TODO: Make sure that text rendering is fine working since boundig boxes are non non inlusive for x1,y1






#endif