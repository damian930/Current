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
  Font_info* font_info = load_font(font_arena, range_u32('!', '~'), 400, Str8FromClit(font_arena, FONT_PATH));

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
            draw_rect(rect_make(200, 200, 500, 500), C_LIGHT_GREEN);
            test_draw_text(font_info, font_texture, str8_temp_from_cstr("255"), 200, 200); 
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








