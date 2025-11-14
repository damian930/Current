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
  DefereLoop(os_win32_init(), os_win32_release())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release())
  {
    EntryPoint();
  }

  return 0;
}

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
              ui_push_background_color(C_LIGHT_GREEN);
              ui_begin_box(UI_SizePx(500), UI_SizePx(500), Axis2_y, "Main key", UI_box_flag__has_backgound, "");
              {
                local B32 is_menu_open = false;
                if (ui_get_inputs().is_clicked)
                {
                  ToggleBool(is_menu_open);
                }
                
                if (is_menu_open)
                {
                  ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, UI_Key(__LINE__), UI_box_flag__has_text, "Text");
                  {}
                  ui_end_box();
                }
              }
              ui_end_box();
            }
            ui_draw_ui();
          }

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








