#include "stdio.h"
#include "stdlib.h"

#include "base/include.h"
#include "base/include.cpp"

#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp"

#include "os/gfx/os_gfx_win32.h"
#include "os/gfx/os_gfx_win32.cpp"

#include "render/render_gl_win32.h"
#include "render/render_gl_win32.cpp"

#include "other/image_stuff/image_loader.h"
#include "other/image_stuff/image_loader.cpp"

#include "font/font.h"
#include "font/font.cpp"

#include "ui/ui_core.h"
#include "ui/ui_core.cpp"

#include "ui/ui_widgets.h"
#include "ui/ui_widgets.cpp"

// Damian: On November 14th the codebase had 4k lines, 700 comments, 1100 blanks.

void EntryPoint();

int main()
{
  // Damian: Since i have layers that i have for the entire run of the application,
  //         i dont want to have all these scopes in the main function code.
  //         So i decided to initialise them up front and then just use then inside a custom
  //         EntryPoint (not my idea, Ryan J. Fleury style).
  DefereLoop(os_win32_state_init(), os_win32_state_release())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release())
  {
    EntryPoint();
  }

  return 0;
}

#include "process_tracker/process_tracker.h"
#include "process_tracker/process_tracker.cpp"
void EntryPoint()
{
  Arena* font_arena = arena_alloc(Megabytes_U64(10), "Font test arena");
  // #define FONT_PATH "../data/papyrus.ttf"
  #define FONT_PATH "../data/Roboto-Regular.ttf"
  Font_info* font_info = load_font(font_arena, range_u32(' ' + 1, '~'), 32, Str8FromClit(font_arena, FONT_PATH));

  Arena* process_arena = arena_alloc(Megabytes_U64(10), "Process arena");
  Process_data_list* list = get_all_process_data(process_arena);
  // for (Process_data_node* node = list->first; node != 0; node = node->next)
  // {
  //   Str8 path = str8_temp_from_str8(node->process_data.path); //str8_temp_from_str8(node->process_data.path);
  //   printf("Path: %s \n", path.data);
  // }

  Arena* image_arena = arena_alloc(Megabytes_U64(30), "Image test arena");
  Image2D jimmy_image = load_png(image_arena, Str8FromClit(image_arena, "../data/jimmy.png"), false);
  Texture2D jimmy_texture = load_texture(jimmy_image);
  
  Win32_window* window = 0;
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  {
    // TODO: Thing about not having ui init take paramets and just set them like for renderer to keep the layer init clean
    DefereLoop(ui_state_init(window, font_info), ui_state_release()) 
    {
      Texture2D font_texture = create_a_texture_from_font_atlas(font_info);
      ui_equip_font_texture(font_texture);
      DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
      {
        r_gl_win32_set_frame_rate(144);
        
        Arena* str_arena = arena_alloc(Kilobytes_U64(64), "String arena");

        // ui_key_bfrom_str8(Str8FromClit(str_arena, "TEST## 1 2 3 ## f"));

        while (!win32_window_shoud_close(window))
        {
          arena_clear(str_arena);

          DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
          {
            DefereLoop(ui_begin_build(), ui_end_build())
            {
              ui_set_padding(50);
              ui_set_background_color(C_BROWN);

              local F32 slider_value = 0.0f;
              ui_slider(Str8FromClit(str_arena, "Slider"), &slider_value, 0, 100);
              printf("Slider value: %f \n", slider_value);

              #if 0 // First test for overflow
              UI_BackgroundColor(C_RED)
              UI_BoxLoop(Str8FromClit(str_arena, "V_Stack"), UI_box_flag__has_backgound, str8_empty())
              {
                ui_set_layout_axis(Axis2_x);
                ui_set_size_x(ui_size_px_make(400, 1));
                ui_set_size_y(ui_size_px_make(400, 1));

                ui_label(Str8FromClit(str_arena, "label"), Str8FromClit(str_arena, "TextTextText"));

                UI_BackgroundColor(C_PURPLE)
                UI_SizeX(ui_size_percent_of_parent_make(1.0f))
                UI_SizeY(ui_size_percent_of_parent_make(1.0f))
                {
                  UI_BoxLoop(Str8FromClit(str_arena, "spacer"), UI_box_flag__has_backgound, str8_empty())
                  {

                  }  
                }  


              }
              #endif



              #if 0 // Clay demo like manu
              UI_BoxLoop(Str8FromClit(str_arena, "V_Stack"), UI_box_flag__has_backgound, str8_empty())
              {
                ui_set_layout_axis(Axis2_y);
                ui_set_padding(10);
                ui_set_child_gap(10);
                ui_set_size_x(ui_size_px_make(400, 1.0f));
                ui_set_size_y(ui_size_child_sum_make());

                char* button_names[] = {"Copyfasdasdfsdfasfasfasfasffasd", "Paste", "Delete", "Comment"};

                ForEach(button_index, button_names)
                {
                  UI_LayoutAxis(Axis2_x)
                  UI_Padding(15)
                  UI_ChildGap(0)
                  UI_SizeX(ui_size_fit_the_parent_make())
                  UI_SizeY(ui_size_child_sum_make())
                  UI_BackgroundColor(C_PURPLE)
                  UI_BoxLoop(Str8FromClit(str_arena, "Button Row 1"), UI_box_flag__has_backgound, str8_empty())
                  {
                    UI_BackgroundColor(C_BLUE)
                    {
                      ui_label("Copy button", str8_from_cstr(str_arena, button_names[button_index]));
                    }
                    ui_spacer(str_arena, Axis2_x);
                    ui_image_pro(Str8FromClit(str_arena, "Image"), jimmy_texture, 50, 50);
                  }
                }

              }
              #endif

              #if 0
              DefereLoop(ui_push_size_x(ui_size_percent_of_parent_make(1)), ui_pop_size_x())
              DefereLoop(ui_push_size_y(ui_size_percent_of_parent_make(1)), ui_pop_size_y())
              DefereLoop(ui_push_background_color(C_GREY), ui_pop_background_color())
              DefereLoop(ui_push_padding(15), ui_pop_padding())
              DefereLoop(ui_push_child_gap(1), ui_pop_child_gap())
              DefereLoop(ui_push_padding_color(C_LIGHT_GREEN), ui_pop_padding_color())
              DefereLoop(ui_push_child_gap_color(C_MAGENTA), ui_pop_child_gap_color())
              DefereLoop(ui_push_layout_axis(Axis2_y), ui_pop_layout_axis())
              UI_BoxLoop(Str8FromClit(str_arena, "Image test"), UI_box_flag__NONE, str8_empty())
              {
                UI_BoxLoop(Str8FromClit(str_arena, "Key v stack"), UI_box_flag__has_backgound|UI_box_flag__draw_padding|UI_box_flag__draw_child_gap, str8_empty())
                {
                  U32 node_index = 0;
                  for (Process_data_node* node = list->first; node != 0; node = node->next, node_index += 1)
                  {
                    DefereLoop(ui_push_size_x(ui_size_child_sum_make()), ui_pop_size_x())
                    // DefereLoop(ui_push_size_y(ui_size_child_sum_make()), ui_pop_size_y())
                    // DefereLoop(ui_push_size_x(ui_size_percent_of_parent_make(1)), ui_pop_size_x())
                    DefereLoop(ui_push_size_y(ui_size_child_sum_make()), ui_pop_size_y())
                    DefereLoop(ui_push_layout_axis(Axis2_x), ui_pop_layout_axis())
                    DefereLoop(ui_push_background_color(C_RED), ui_pop_background_color())
                    UI_BoxLoop(Str8FromClit(str_arena, "Row"), UI_box_flag__has_backgound, str8_empty())
                    {
                      DefereLoop(ui_push_background_color(C_BROWN), ui_pop_background_color())
                      DefereLoop(ui_push_padding(0), ui_pop_padding())
                      UI_BoxLoop(Str8FromClit(str_arena, "Image and name box"), UI_box_flag__has_backgound, str8_empty())
                      {
                        ui_image_pro(Str8FromClit(str_arena, "Image key"), jimmy_texture, 50, 50);
                        DefereLoop(ui_push_background_color(C_BLUE), ui_pop_background_color())
                        {
                          ui_label(Str8FromClit(str_arena, "row key"), get_file_basename(node->process_data.path));
                        }
                      }
                      
                      ui_spacer(Axis2_x);
                      
                      DefereLoop(ui_push_background_color(C_BLUE), ui_pop_background_color())
                      {
                        ui_label(Str8FromClit(str_arena, "row key"), time_as_str8(str_arena, node->process_data.creation_time));
                      }
                      
                    }
                  }
                  
                }
              }
              #endif
            }
            ui_draw_ui();
          } 
            
        } 
      } 

    }
  }
}

#if 0
void EntryPoint()
{
  Arena* font_arena = arena_alloc(Megabytes_U64(10), "Font test arena");
  // #define FONT_PATH "../data/papyrus.ttf"

  #define FONT_PATH "../data/Roboto-Regular.ttf"
  Font_info* font_info = load_font(font_arena, range_u32('!', '~'), 52, Str8FromClit(font_arena, FONT_PATH));

  // for (Font_kern_node* node = font_info->kern_list.first; node != 0; node = node->next)
  // {
    // printf("(%c), (%c), (%f) \n", node->kern_pair.codepoint1, node->kern_pair.codepoint2, node->kern_pair.advance);
  // }

  Win32_window* window = 0;
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  {
    // TODO: Thing about not having ui init take paramets and just set them like for renderer to keep the layer init clean
    DefereLoop(ui_state_init(window, font_info), ui_state_release()) 
    {
      Texture2D font_texture = create_a_texture_from_font_atlas(font_info);
      ui_equip_font_texture(font_texture);
      DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
      {
        r_gl_win32_set_frame_rate(144);
        
        while (!win32_window_shoud_close(window))
        {
          DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
          {
            DefereLoop(ui_begin_build(), ui_end_build())
            {
              // TODO: Test 
              ui_push_background_color(C_LIGHT_GREEN);

              ui_begin_box(UI_SizePercentOfParent(1), UI_SizeChildrenSum(), Axis2_x, "Name row", UI_box_flag__has_backgound, "");
              {
                UI_BackgroundColor(C_BLUE)
                {
                  ui_begin_box(UI_SizeChildrenSum(), UI_SizeChildrenSum(), Axis2_x, "Box 1", UI_box_flag__has_backgound, "");
                  {
                    ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "Icon text", UI_box_flag__has_text, "Icon");
                    {}
                    ui_end_box();
                  
                    ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "Title text", UI_box_flag__has_text, "Title");
                    {}
                    ui_end_box();
                  }
                  ui_end_box();
                }
                
                // Spacer here to stretch to the sides of the screen
                ui_begin_box(UI_SizeFitTheParent(), UI_SizePx(0), Axis2_x, "Spacer 1 key", UI_box_flag__NONE, "");
                {}
                ui_end_box();

                UI_BackgroundColor(C_RED)
                {
                  UI_ChildGap(50)
                  {
                    ui_begin_box(UI_SizeChildrenSum(), UI_SizeChildrenSum(), Axis2_x, "Box 1", UI_box_flag__has_backgound, "");
                    {
                      ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "CPU text", UI_box_flag__has_text, "CPU");
                      {}
                      ui_end_box();
                      
                      ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "GPU text", UI_box_flag__has_text, "GPU");
                      {}
                      ui_end_box();
                      
                      ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, "RAM text", UI_box_flag__has_text, "RAM");
                      {}
                      ui_end_box();
                    }
                    ui_end_box();
                  }
                  ui_pop_child_gap();
                }

              }
              ui_end_box();

              // ui_begin_box(UI_SizePx(500), UI_SizePx(500), Axis2_y, "Main key", UI_box_flag__has_backgound, "");
              // {
              //   local B32 is_menu_open = false;
              //   if (ui_get_inputs().is_clicked)
              //   {
              //     ToggleBool(is_menu_open);
              //   }
                
              //   if (is_menu_open)
              //   {
              //     ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, UI_Key(__LINE__), UI_box_flag__has_text, "Text");
              //     {}
              //     ui_end_box();
              //   }
              // }
              // ui_end_box();
            }
            ui_draw_ui();
          }

        }
        
      }
    }
  }
} 

#endif

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








