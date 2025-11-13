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

#include "ui/ui_core.h"
#include "ui/ui_core.cpp"

#include "ui/ui_widgets.h"

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

#include "font/font.h"
#include "font/font.cpp"

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
  DefereLoop(os_win32_init(), os_win32_release())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release()) 
  {
    Arena* font_arena = arena_alloc(Megabytes_U64(10), "Font test arena");
    // #define FONT_PATH "../data/papyrus.ttf"
    #define FONT_PATH "../data/Roboto-Regular.ttf"
    Font_info* font_info = load_font(font_arena, range_u32('!', '~'), 52, Str8FromClit(font_arena, FONT_PATH));
    
    DefereLoop(ui_state_init(window, font_info), ui_state_release()) 
    {
      Texture2D font_texture = create_a_texture_from_font_atlas(font_info);
      ui_equip_font_texture(font_texture);
      DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
      {
        r_gl_win32_set_frame_rate(144);
        load_char_textures();
        
        // text_layer_release(); 
        // Damian: Remove this to not worry about this for now
        
        
        // for (Font_codepoint_data_node* node = font_info->hash_list->first; 
        //      node != 0; 
        //      node = node->next
        // ) {
          //   printf("%c: (%d, %d), (%d, %d) \n", node->codepoint, node->codepoint_offset.x0, node->codepoint_offset.y0, node->codepoint_offset.x1, node->codepoint_offset.y1);
          // }
          
          // for (Font_kern_node* node = font_info->kern_list.first; node != 0; node = node->next)
          // {
            // printf("C1: %c, C2: %c, Add: %f \n", node->kern_pair.codepoint1, node->kern_pair.codepoint2, node->kern_pair.advance);
            // }
            
            
            // stbi_write_png("test_font_atlas_bitmap.png", font_info->font_atlas.width, font_info->font_atlas.height, 1, font_info->font_atlas.data_buffer.data, 0);
            
            // Rect rect = font_measure_text(font_info, Str8FromClit(font_arena, "Flopper S[]"));
            // printf("Font measure: W-->%f, H-->%f \n", rect.width, rect.height);
            
            while (!win32_window_shoud_close(window))
            {
              DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
              {
                // test_draw_text(font_info, font_texture, Str8FromClit(font_arena, "Flopper S[]"), 0, 0);
                // draw_rect(rect, color_make(1.0f, 0, 0, 0.3));
                
                // local B32 is_draw = false;
                
                auto row_test = [](const char* id, Color color, const char* app_name, const char* time) {
                  ui_h_stack(id)
                  {
                    // TODO: These dont work at all
                    ui_set_max_size(800, Axis2_x);
                    ui_set_min_size(300, Axis2_x);
                    
                    ui_set_backgound_color(color);
                    ui_label(app_name, id);
                    ui_spacer();
                    ui_label(time, id);
                    
                    if(ui_is_clicked())
                    {
                      printf("%s \n", app_name);
                    }
                  }
                };
                
                ui_begin_build();
                {
                  
                  ui_v_stack("V stack id")
                  {
                    row_test("1", C_LIGHT_GREEN, "Telegram", "2:55");
                    row_test("2", C_BROWN, "Fortnite", "3:14");
                    row_test("3", C_LIGHT_GREEN, "The finals", "1:15");
                    row_test("4", C_BROWN, "Hearthstone", "4:12");
                  }
                  
                  
                  //   local B32 b_s[3] = {};
                  //   local Color color_arr[3] = {C_LIGHT_GREEN, C_PURPLE, C_BROWN};
                  //   local U32 color_index = 0;
                  //   ui_checkbox_entry(&b_s[0], &color_index, 0, "Entry 1", "Entry key 1");
                  //   ui_checkbox_entry(&b_s[1], &color_index, 1, "Entry 2", "Entry key 2");
                  //   ui_checkbox_entry(&b_s[2], &color_index, 2, "Entry 3", "Entry key 3");
                  
                  //   if (ui_button(color_arr[color_index], "Button", "Button key", "Label key"))
                  //   {
                    //     Color color = color_arr[color_index];
                    //     printf("Color: (%d, %d, %d) \n", (U32)(color.r * 255), (U32)(color.g * 255), (U32)(color.b * 255));
                    //   }
                    
                    
                    
                    
                    
                    // ui_push_padding(0);
                    // ui_push_child_gap(0);
                    // // ui_draw_child_gap_color(C_WHITE);
                    // // ui_draw_padding_for_current(C_GREY);
                    
                    // ui_begin_box(UI_SizeChildrenSum(), UI_SizeChildrenSum(), Axis2_y, "Clay like box", UI_box_flag__has_backgound, C_ORANGE, "");
                    // {
                      //   ui_set_min_size(400, Axis2_x);
                      //   ui_set_max_size(800, Axis2_x);
                      
                      //   // ui_begin_box(UI_SizeFitTheParent(), UI_SizePx(10), Axis2_x, "Fit 1", UI_box_flag__has_backgound, C_LIGHT_GREEN, "");
                      //   // {}
                      //   // ui_end_box();
                      
                      //   ui_begin_box(UI_SizeChildrenSum(), UI_SizeChildrenSum(), Axis2_x, "Row 1", UI_box_flag__has_backgound, C_PINK, "");
                      //   {
                        //     // ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "Tect for row 1 1", UI_box_flag__has_text, C_TRANSPARENT, "Copy");
                        //     // {}
                        //     // ui_end_box();
                        
                        //     ui_begin_box(UI_SizeFitTheParent(),UI_SizeFitTheParent(), Axis2_x, "Fit 1", UI_box_flag__NONE, C_LIGHT_GREEN, "");
                        //     {}
                        //     ui_end_box();
                        
                        //     // ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "Tect for row 1 2", UI_box_flag__has_text, C_TRANSPARENT, "IMAGE");
                        //     // {}
                        //     // ui_end_box();  
                        //   }
                        //   ui_end_box();
                        
                        // }
                        // ui_end_box();
                        
                        
                      }
                      ui_end_build();
                      
                      ui_draw_ui();
                      
                    }
        }
      }
    }
  }
  return 0;
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








