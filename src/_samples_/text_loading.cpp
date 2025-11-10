#include <stdio.h>

#include "base/include.h"
#include "base/include.cpp"

#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp"

#include "other/image_stuff/image_loader.h"
#include "other/image_stuff/image_loader.cpp"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb/stb_image_write.h"

void EntryPoint();
int main()
{
  DefereLoop(DEBUG_win32_init(), DEBUG_win32_release())
  {
    EntryPoint();
  }

  return 0;
}

// =========================================================

// Configure a supported range of codepoints
// Create a font map using it (Data_buffer)
// Create a hash map from a supported codepoint to coordinates for a texture for that codepoint
// Store additional data related to fonts (all the ttf font measuring data that might be usefull later)
// Data to store:
//  -- Number of fonts in a ttf file 
//  -- Is codepoint supported
//  -- Ascent, Descent, LineGap
//  -- OS/2[Ascent, Descent, LineGap]
//  -- LeftSideBearing, AdvanceWidth
//  -- KernAdvance for 2 codepoints
//  -- 

struct Font_atlas_codepoint_offset {
  U32 x0;
  U32 y0;
  U32 x1;
  U32 y1;
};

// TODO: This need a better name for sure
struct Font_hash_node {
  Font_hash_node* next;
  Font_hash_node* prev;
  
  U8 codepoint;
  F32 advance_width;
  F32 left_side_bearing;
  Font_atlas_codepoint_offset codepoint_offset;
};

struct Font_hash_list {
  Font_hash_node* first;
  Font_hash_node* last;
  U32 node_count;
};

struct Font_info {
  Font_hash_list hash_list[1];

  Image2D font_atlas;
  U32 font_size;
  F32 scale;
  F32 ascent;
  F32 descent;
  F32 line_gap;
};

global Font_info g_font_info = {};

void load_font(Arena* arena, Range_U32 range_of_codepoints, U32 font_size)
{
  Scratch scratch = get_scratch();
  {
    // Getting the ttf data, Creating the font
    Str8 ttf_file_str8 = str8_from_cstr(scratch.arena, "../../data/Roboto-Regular.ttf");    
    Data_buffer ttf_data = read_file_inplace(scratch.arena, ttf_file_str8);

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
    g_font_info.font_atlas = font_atlas;
    g_font_info.font_size = font_size;

    F32 scale = stbtt_ScaleForPixelHeight(&font_info, font_size);
    g_font_info.scale = scale; 
    {
      int ascent  = {};
      int descent = {};
      int linegap = {};
      stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &linegap);
      g_font_info.ascent  = (F32)ascent  * scale;
      g_font_info.descent = (F32)descent * scale;
      g_font_info.line_gap = (F32)linegap * scale;
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
      Font_hash_list list = g_font_info.hash_list[codepoint_hash];
      
      for (Font_hash_node* node = list.first; node != 0; node = node->next)
      {
        AssertNote(node->codepoint == it_codepoint, "Hash map cant contain more than 1 enty with the same key.");
      }

      U32 codepoint_pached_data_index = it_codepoint - range_of_codepoints.min;
      stbtt_packedchar* codepoint_pached_data = glyph_metrics + codepoint_pached_data_index;

      Font_hash_node* new_node = ArenaPush(arena, Font_hash_node);
      new_node->codepoint         = it_codepoint;
      new_node->advance_width     = advance_width;
      new_node->left_side_bearing = left_side_bearing;
      new_node->codepoint_offset  = Font_atlas_codepoint_offset{codepoint_pached_data->x0, 
                                                                codepoint_pached_data->y0, 
                                                                codepoint_pached_data->x1, 
                                                                codepoint_pached_data->y1};
      DllPushBack(&list, new_node);
      list.node_count += 1;
    }

    // // TODO: See if OS/2 ascent,descent... are needed
    // int advance_width = {};
    // int left_side_bearing = {};
    // stbtt_GetCodepointHMetrics(&font_info, codepoint, &advance_width, &left_side_bearing);
  }
  end_scratch(&scratch);

}

void EntryPoint()
{
  Scratch scratch = get_scratch();
  {
    load_font(scratch.arena, range_u32('!', '~' + 1), 100);
    delete_file(Str8FromClit(scratch.arena, "test_font_atlas_bitmap.png"));
    stbi_write_png("test_font_atlas_bitmap.png", g_font_info.font_atlas.width, g_font_info.font_atlas.height, 1, g_font_info.font_atlas.data_buffer.data, 0);
  }
  end_scratch(&scratch);
}

    
    
    