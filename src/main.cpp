#include "stdio.h"
#include "stdlib.h"

#include "base/include.h"
#include "base/include.cpp"

#include "os/core/os_core_win32.h"
// #include "os/core/os_core_win32.cpp"

#include "os/gfx/os_gfx_win32.h"
#include "os/gfx/os_gfx_win32.cpp"

#include "render/render_gl_win32.h"
#include "render/render_gl_win32.cpp"

#include "other/image_stuff/image_loader.h"
#include "other/image_stuff/image_loader.cpp"

// CreateCustonWindowDrawF(window_frame_draw_f) {
//   DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
//   {
//     DEBUG_draw_rect_begin(Rect{100, 100, 100, 100});
    
//   }
// }

/* TODO:
 -- Some keyboard inputs
 -- Some mouse inputs
 -- Multiple windows
 -- Render for multiple windows
*/

struct Pair_char_texture {
  U8 ch;
  Image2D bitmap;
  B32 has_texture;
  Texture2D texture;
};

global Arena* font_perm_arena                    = arena_alloc(Gigabytes_U64(1), "Font perm arena");
global Pair_char_texture char_texture_pairs[128] = {};
global stbtt_fontinfo font_info                  = {};

void load_char_textures()
{
  Data_buffer font_data = {};
  {
    Scratch scratch = get_scratch();
    Win32_file ttf_file = open_file(Str8FromClit(scratch.arena, "../data/Roboto-Regular.ttf"), File_access_flag_read);
    font_data = read_file(scratch.arena, ttf_file);  
    close_file(ttf_file);
    end_scratch(&scratch);
  }
  Assert(font_data.count > 0);

  int tff_font_index = stbtt_InitFont(&font_info, font_data.data, 0);
  Assert(tff_font_index != 0);

  for (U32 codepoint = 0; codepoint < 128; codepoint += 1)
  {
    Pair_char_texture* pair = char_texture_pairs + codepoint;
    pair->ch = codepoint;
    Scratch scratch = get_scratch();
    {
      Image2D font_image = create_bitmap_for_char_rgba(font_perm_arena, &font_info, codepoint, 100);
      pair->bitmap = font_image;
      if (font_image.data_buffer_opt.count > 0) {
        pair->has_texture = true;
        pair->texture = load_texture(font_image);
      }
    }
    end_scratch(&scratch);
  }
}

Pair_char_texture get_char_texture(U8 codepoint)
{
  Pair_char_texture result = {};
  for (U32 test_codepoint = 0; test_codepoint < 128; test_codepoint += 1)
  {
    if (test_codepoint == codepoint) {
      result = char_texture_pairs[codepoint];
      break;
    }
  }  
  return result;
}

// TODO: Remove the extern forlde rand just create a "3rd-party" folder and put all the STBs in there
int main()
{
  // #ifdef UNICODE 
  //   printf("UNICODE \n");
  // #else
  //   printf("ASCII \n");
  // #endif

  // // The text in my application never changes so the whole string is rendered to txBuffer
  // // after precomputing its dimensions: txWidth and txHeight.
  // txBuffer.clear();
  // txBuffer.resize(txWidth * txHeight);
  // for (size_t i = 0; i < text.length(); i++) {
  //     int lsb, dx, dy = (ascent - descent) + lineGap;
  //     int kern = stbtt_GetCodepointKernAdvance(fontInfo, text[i], text[i+1]);
  //     stbtt_GetCodepointHMetrics(fontInfo, text[i], &dx, &lsb);
  //     int x1, y1, x2, y2;
  //     stbtt_GetCodepointBitmapBox(fontInfo, text[i], scale, scale, &x1, &y1, &x2, &y2);
  //     size_t offset = x + (lsb * scale) + ((y + ascent + y1) * txWidth);
  //     stbtt_MakeCodepointBitmap(fontInfo, &txBuffer[offset], x2 - x1, y2 - y1, txWidth, scale, scale, text[i]);
  //     x += (dx + kern) * scale;
  // }

  Win32_window* window = 0;
  DefereLoop(DEBUG_win32_init(), DEBUG_win32_end())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release()) 
  {
    DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
    {
      r_gl_win32_set_frame_rate(144);
      load_char_textures();

      while (!win32_window_shoud_close(window))
      {
        DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
        {

          auto test_draw_text = [](char* str, F32 x, F32 y) {
            for (int i = 0; i < cstr_len(str); i += 1)
            {
              Pair_char_texture ct = get_char_texture(str[i]);
              if (ct.has_texture) {
                draw_rect(rect_make(x, y, ct.texture.width, ct.texture.height), C_RED);
                test_draw_texture(ct.texture, x, y);
                F32 gap = 0.0f;
                x += ct.texture.width + gap;
              }
            }
          };

          // draw_rect(rect_make(100, 100, 100, 100), C_RED);
          // draw_rect(rect_make(100, 100, 100, 100), vec4_f32(0, 0, 1, 0.5));

          test_draw_text("Flopper.", 100, 100);

          // draw_rect(rect_make(100, 100, 100, 100));

          if (is_key_clicked(window, Key_a)) {
            printf("Exit on a key. \n");
            exit(1);
          }
        }
  
      }
    }

  }

  #define SCREEN_SHOT_DEMO 0
  #if SCREEN_SHOT_DEMO
  {
    local_persist B32 screen_shot_taken = false;
    if (!screen_shot_taken)
    {
      screen_shot_taken = true;
      Scratch scratch = get_scratch();
      {
        Data_buffer screenshot = win32_make_screen_shot(scratch.arena);
  
        U32 flags = File_access_flag_read|File_access_flag_append|File_access_flag_share_read; 
        Win32_file other_file = open_file(Str8FromClit(scratch.arena, "screen_shot_test.bmp"), flags); 
        {
          write_to_file(other_file, screenshot);
        }
        close_file(other_file);
  
      }
      end_scratch(&scratch);
    }
  }
  #endif

  return 0;
}







