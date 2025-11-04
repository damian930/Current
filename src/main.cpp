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

int main()
{
  // #ifdef UNICODE 
  //   printf("UNICODE \n");
  // #else
  //   printf("ASCII \n");
  // #endif

  Win32_window* window = 0;
  DefereLoop(DEBUG_win32_init(), DEBUG_win32_end())
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  DefereLoop(r_gl_win32_state_init(), r_gl_win32_state_release()) 
  {
    DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
    {
      r_gl_win32_set_frame_rate(144);
      while (!win32_window_shoud_close(window))
      {
        DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
        {
   
          draw_rect(rect_make(10, 30, 100, 200));
  
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







