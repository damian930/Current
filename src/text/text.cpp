#ifndef D_TECT_CPP
#define D_TECT_CPP

// TODO: Remove this
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb/stb_image_write.h"

#include "other/image_stuff/image_loader.cpp"

#include "text.h"
#include "base/arena.cpp"
#include "os/core/os_core_win32.h"

/* TODO:
  -- Load a font and create a font atlas to use
  -- Load the font atals to he gpu and store its data to then crop the chars out
  -- Then render the text using it 
  
*/

void text_layer_init()
{
  Arena* arena = arena_alloc(Gigabytes_U64(1), "Text state arena");
  g_text_layer_state = ArenaPush(arena, Text_layer_state);
  g_text_layer_state->perm_state_arena = arena;
}

void text_layer_release()
{
  arena_release(g_text_layer_state->perm_state_arena);
  g_text_layer_state = 0;
}

// ---

void text_prepare_font(const char* ttf_file_path, U32 font_size)
{
  Arena* arena = g_text_layer_state->perm_state_arena;
  stbtt_fontinfo font_info    = {};
  U32 font_atlas_width        = 1024;
  U32 font_atlas_height       = 1024;
  Data_buffer font_atlas_data = {};
  {
    Range_U32 asci_supported_range = range_u32(0, 128);
    U32 count = range_u32_count(asci_supported_range);

    Str8 ttf_file_str8 = str8_from_cstr(arena, ttf_file_path);    
    Data_buffer ttf_data = read_file_inplace(arena, ttf_file_str8);
    Assert(ttf_data.count > 0); // TODO: This has to go

    stbtt_packedchar* glyph_metrics = ArenaPushArr(arena, stbtt_packedchar, count);
    stbtt_pack_range ranges = {(F32)font_size, 
                               (int)asci_supported_range.min, 
                               NULL, 
                               (int)count, 
                               glyph_metrics, 
                               Null, Null};
    font_atlas_data = data_buffer_make(arena, font_atlas_width * font_atlas_height);

    stbtt_pack_context pc = {};
    stbtt_PackBegin(&pc, font_atlas_data.data, font_atlas_width, font_atlas_height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1); 
    stbtt_PackFontRanges(&pc, ttf_data.data, 0, &ranges, 1);
    stbtt_PackEnd(&pc);
    
    stbtt_InitFont(&font_info, ttf_data.data, stbtt_GetFontOffsetForIndex(ttf_data.data, 0));
    
    F32 scale     = stbtt_ScaleForPixelHeight(&font_info, ranges.font_size);
    F32 ascent  = {};
    F32 descent = {};
    F32 linegap = {};
    {
      int int_ascent  = {};
      int int_descent = {};
      int int_linegap = {};
      stbtt_GetFontVMetrics(&font_info, &int_ascent, &int_descent, &int_linegap);
      ascent  = int_ascent * scale;
      descent = int_descent * scale;
      linegap = int_linegap * scale;
    }

    // stbi_write_png("test_font_atlas_bitmap.png", font_atlas_width, font_atlas_height, 1, font_atlas_data.data, 0);
  }

  g_text_layer_state->current_font_data = ArenaPush(arena, Font_data);
  Font_data* font_data  = g_text_layer_state->current_font_data;

  font_data->font_info         = font_info;
  font_data->font_size         = font_size;
  font_data->font_atlas_width  = font_atlas_width;
  font_data->font_atlas_height = font_atlas_height;
  font_data->font_atlas_data   = font_atlas_data; 
}

void text_load_font()
{
  Assert(g_text_layer_state);
  AssertNote(g_text_layer_state->current_font_data, "Load onto the gpu is only valid when we have the font atlas ready.");
  
  Arena* arena = g_text_layer_state->perm_state_arena;
  Font_data* font_data = g_text_layer_state->current_font_data;

  Image2D bitmap = {}; 
  {
    U32 h = font_data->font_atlas_width;
    U32 w = font_data->font_atlas_height;
    
    U64 input_data_size = w * h * 1; // Damian: stb only uses 1 byte per pixel in "stbtt_GetCodepointBitmap"
    U64 my_data_size    = w * h * 4; // Damian: RGBA
    Data_buffer my_data = data_buffer_make(arena, my_data_size);
  
    U8* image_byte    = font_data->font_atlas_data.data;
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
    bitmap.width           = w;
    bitmap.height          = h;
    bitmap.n_chanels       = 4;
    bitmap.data_buffer_opt = my_data;
  }

  font_data->is_texture_present = true;
  font_data->font_texture = load_texture(bitmap);
}

// ---

Vec2_F32 text_get_text_dims(Str8 str)
{
  F32 text_height = 


}



#endif