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

#include "ui/ui.h"
#include "ui/ui.cpp"

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

// - Base line for text
// - Get from stb all the things need to render it for base line (h, w, offsets ...)
// - Do something with font better
// - Render some text 

#include "text/text.h"
#include "text/text.cpp"

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

  Arena* arena = arena_alloc(Kilobytes_U64(10), "Test arena");
  Str8 str = str8_from_cstr(arena, "Flopper");
  // pritnf("Str: %s \n", str.data);
  arena_release(arena);

  Win32_window* window = 0;
  DefereLoop(DEBUG_win32_init(), DEBUG_win32_release())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release()) 
  DefereLoop(ui_state_init(window), ui_state_release()) 
  {
    DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
    {
      r_gl_win32_set_frame_rate(144);
      load_char_textures();

      // text_layer_release(); 
      // Damian: Remove this to not worry about this for now

      Arena* font_arena = arena_alloc(Megabytes_U64(10), "Font test arena");
      #define FONT_PATH "../data/papyrus.ttf"
      // #define FONT_PATH "../data/Roboto-Regular.ttf"
      Font_info* font_info = load_font(font_arena, range_u32('!', '~'), 100, Str8FromClit(font_arena, FONT_PATH));

      // for (Font_codepoint_data_node* node = font_info->hash_list->first; 
      //      node != 0; 
      //      node = node->next
      // ) {
      //   printf("%c: (%d, %d), (%d, %d) \n", node->codepoint, node->codepoint_offset.x0, node->codepoint_offset.y0, node->codepoint_offset.x1, node->codepoint_offset.y1);
      // }

      for (Font_kern_node* node = font_info->kern_list.first; node != 0; node = node->next)
      {
        printf("C1: %c, C2: %c, Add: %f \n", node->kern_pair.codepoint1, node->kern_pair.codepoint2, node->kern_pair.advance);
      }

      Texture2D font_texture = create_a_texture_from_font_atlas(font_info);

      // stbi_write_png("test_font_atlas_bitmap.png", font_info->font_atlas.width, font_info->font_atlas.height, 1, font_info->font_atlas.data_buffer.data, 0);

      while (!win32_window_shoud_close(window))
      {
        DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
        {
          test_draw_text(font_info, font_texture, Str8FromClit(font_arena, "Flopper S[]"), 100, 100);

          ui_begin_build();
          {
            local B32 is_draw = false;
            ui_begin_box(UI_SizePx(20), UI_SizePx(20), Axis2_x, C_WHITE, "TEST BUTTON");
            {
              if (ui_is_clicked())
              {
                ToggleBool(is_draw);
              }
            }
            ui_end_box();

            if (false)
            {
              ui_begin_box(UI_SizePx(200), UI_SizeChildrenSum(), Axis2_y, C_WHITE, "Key 1");
              { 
                ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, C_RED, "Key Text");
                {
                  
                }
                ui_end_box();
               
                ui_begin_box(UI_SizePercentOfParent(1), UI_SizePx(50), Axis2_x, C_GREEN, "Key 2");
                {
                  if (ui_is_clicked())
                  {
                    printf("Key 2 \n");
                  }
                }
                ui_end_box();
  
                ui_begin_box(UI_SizePercentOfParent(0.66), UI_SizePx(100), Axis2_x, C_BLUE, "Key 3");
                {
                  
                }
                ui_end_box();
              }
              ui_end_box();
            }
          }
          ui_end_build();
            
          ui_draw_ui();
        }



          // test_draw_texture(g_text_layer_state->current_font_data->font_texture, 100, 100);

          // Get a texture
          // Align it to the baseline
          // Render 

          // auto test_draw_text = [](U8 ch , F32 baseline_x, F32 baseline_y) -> U32 {
          //   F32 scale = stbtt_ScaleForPixelHeight(&font_info, 100);
            
          //   int ascent = {};
          //   int descent = {};
          //   int line_gap = {};
          //   {
          //     stbtt_GetFontVMetrics(&font_info, &ascent, &descent, &line_gap);
          //     ascent *= scale;
          //     descent *= scale;
          //     line_gap *= scale;
          //   }

          //   int advance_width = {};
          //   int lsb = {};
          //   {
          //     stbtt_GetCodepointHMetrics(&font_info, ch, &advance_width, &lsb);
          //     advance_width *= scale;
          //     lsb *= scale;
          //   }

          //   int x0 = {};
          //   int x1 = {};
          //   int y0 = {};
          //   int y1 = {};
          //   stbtt_GetCodepointBitmapBox(&font_info, ch, 
          //                               scale, scale, 
          //                               &x0,&y0,&x1,&y1);
          //   // 1 14 -15 1
          //   int bbox_width = x1 - x0;
          //   int bbox_height = y1 - y0;
            
          //   // Finals
          //   int f_x0 = baseline_x + x0;
          //   int f_x1 = f_x0 + (x1 - x0);
          //   int f_y0 = baseline_y + y0;
          //   int f_y1 = f_y0 + (y1 - y0);

          //   Pair_char_texture a_texture = get_char_texture(ch);
          //   Assert(a_texture.has_texture);
          //   draw_rect(rect_make(baseline_x, baseline_y - ascent, 1, baseline_y - descent), C_GREEN);
          //   draw_rect(rect_make(baseline_x, baseline_y, 100, 1), C_RED);
          //   draw_rect(rect_from_points(f_x0, f_y0, f_x1, f_y1), C_BLUE);
          //   test_draw_texture(a_texture.texture, f_x0, f_y0);
            
          //   Assert(advance_width >= 0);
          //   return advance_width;
          // };

          // // draw_rect(rect_make(100, 100, 100, 100), C_RED);
          // // draw_rect(rect_make(100, 100, 100, 100), vec4_f32(0, 0, 1, 0.5));

          // // test_draw_text("Flopper.", 100, 100);

          // local U64 current_len = 0;
          // local U64 dyn_str_len = 50;
          // local U8* dyn_str = (U8*)malloc(dyn_str_len);
          // auto push_on_text = [](char ch) {
          //   if (current_len < dyn_str_len) {
          //     dyn_str[current_len++] = ch;
          //   }
          // };

          // if (is_key_clicked(window, Key_a))
          // {
          //   push_on_text('a');
          // }
          // if (is_key_clicked(window, Key_w))
          // {
          //   push_on_text('w');
          // }
           
          // void draw_text(Str8 str, F32 x, F32 y, U32 font_size);
          
          // draw_text('a', 100, 100, )

          // F32 baseline_x = 100;
          // F32 baseline_y = 100; 
          // ForEachEx(char_index, current_len, dyn_str)
          // {
          //   U8* ch = dyn_str + char_index;
          //   U32 width_advance = test_draw_text(*ch, baseline_x, baseline_y);
          //   baseline_x += width_advance;
          // }

          // if (is_key_clicked(window, Key_a)) {
          //   printf("Exit on a key. \n");
          //   exit(1);
          // }
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








