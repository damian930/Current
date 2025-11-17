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
  Font_info* font_info = load_font(font_arena, range_u32('!', '~'), 10, Str8FromClit(font_arena, FONT_PATH));

  Arena* process_arena = arena_alloc(Megabytes_U64(10), "Process arena");
  Process_data_list* list = get_all_process_data(process_arena);
  {
    for (Process_data_node* node = list->first; node != 0; node = node->next)
    {
      Str8 path = str8_temp_from_str8(node->process_data.path); //str8_temp_from_str8(node->process_data.path);
      printf("Path: %s \n", path.data);
    }
  }
  
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

        while (!win32_window_shoud_close(window))
        {
          DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
          {
            #if 1
            DefereLoop(ui_begin_build(), ui_end_build())
            {
              ui_push_background_color(C_LIGHT_GREEN);

              ui_begin_box(ui_size_child_sum_make(), ui_size_child_sum_make(), Axis2_y, "Key", UI_box_flag__has_backgound, Str8FromClit(str_arena, ""));
              {
                for (Process_data_node* node = list->first; node != 0; node = node->next)
                {
                  ui_begin_box(ui_size_child_sum_make(), ui_size_child_sum_make(), Axis2_x, "Row", UI_box_flag__has_backgound, Str8FromClit(str_arena, ""));
                  {
                    ui_begin_box(ui_size_text_make(), ui_size_text_make(), Axis2_x, "Key", UI_box_flag__has_text, node->process_data.path);
                    {} 
                    ui_end_box();

                    // UI_BackgroundColor(C_BLUE)
                    // {
                    //   ui_begin_box(ui_size_fit_the_parent(), ui_size_px_make(1.0f), Axis2_x, "Spacer", UI_box_flag__has_backgound, Str8FromClit(str_arena, ""));
                    //   {} 
                    //   ui_end_box();
                    // }
                  }
                  ui_end_box();

                  // UI_BackgroundColor(C_GREY)
                  // {
                  //   ui_begin_box(ui_size_fit_the_parent(), ui_size_px_make(2.0f), Axis2_x, "Key", UI_box_flag__has_backgound, Str8FromClit(str_arena, ""));
                  //   {} 
                  //   ui_end_box();
                  // }
                }
              }
              ui_end_box();

            } // ui_end_build()
            #endif

            ui_draw_ui();

          } // r_gl_win32_end_frame()
        } // while (!win32_window_shoud_close(window))

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








