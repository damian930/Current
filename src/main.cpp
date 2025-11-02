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

  // {
  //   Scratch scratch = get_scratch();
  //   {
  //     load_png(scratch.arena, Str8FromClit(scratch.arena, "data/test.pn"));
  //   }
  //   end_scratch(&scratch);


  //   return 0;
  // }

  // Scratch scratch = get_scratch();
  // {
  //   Str8 str = Str8FromClit(scratch.arena, "F/main.cpp");
  //   Str8 str_nt = str8_from_str8_temp_null_term(scratch.arena, str); 
  //   printf("Str: %s \n", str_nt.data);

  //   Str8 name = get_file_extension(str);
  //   Str8 name_nt = str8_from_str8_temp_null_term(scratch.arena, name); 
  //   printf("Name: %s \n", name_nt.data);

  //   // Str8 path_end = list.last->str;
  //   // Str8_list dot_separated = str8_split_by_char(scratch.arena, path_end, '.');
  //   // Str8 ext = dot_separated.last->str;
  //   // Str8 ext_nt = str8_from_str8_temp_null_term(scratch.arena, ext);
  //   // printf("Ext: %s \n", ext_nt.data);
  // }
  // end_scratch(&scratch);
  // return 0;

  DefereLoop(DEBUG_win32_init(), DEBUG_win32_end())
  {
  DefereLoop(win32_gfx_init(), win32_gfx_release())
  {
  Win32_window* window;
  DefereLoop(window = win32_create_window(), win32_close_window(window)) 
  {
  DefereLoop(r_gl_win32_init(), r_gl_win32_end()) 
  {
  DefereLoop(r_gl_win32_equip_window(window), r_gl_win32_remove_window()) 
  { 
    r_gl_win32_set_frame_rate(144);
    r_gl_win32_set_screen_origin_top_left();

    // TODO: See if you are fine with the origin pointe of the rect 
    //       Would make more sense to move it to be from the bottom left when y goes up.

    struct {
      Rect main_rect;
    } state = {}; 
    state.main_rect = rect_make(100, 100, 100, 100);

    while (!win32_window_shoud_close(window))
    {
      DefereLoop(r_gl_win32_begin_frame(), r_gl_win32_end_frame())
      {
        // Update the state
        {
          if (is_key_clicked(window, Key_a)) {
            state.main_rect.x -= 5;
          }          
          if (is_key_clicked(window, Key_d)) {
            state.main_rect.x += 5;
          }

          if (is_key_clicked(window, Key_s)) {
            state.main_rect.y += 5;
          }          
          if (is_key_clicked(window, Key_w)) {
            state.main_rect.y -= 5;
          }


        }

        // Draw calls
        {
          draw_rect(state.main_rect);
        }
      }

    }

  }
  }
  }
  }
  }
  
  {
    {


      // local_persist B32 screen_shot_taken = false;
      // if (!screen_shot_taken)
      // {
      //   screen_shot_taken = true;
      //   Scratch scratch = get_scratch();
      //   {
      //     Data_buffer screenshot = win32_make_screen_shot(scratch.arena);

      //     U32 flags = File_access_flag_read|File_access_flag_append|File_access_flag_share_read; 
      //     Win32_file other_file = open_file(Str8FromClit(scratch.arena, "screen_shot_test.bmp"), flags); 
      //     {
      //       write_to_file(other_file, screenshot);
      //     }
      //     close_file(other_file);

      //   }
      //   end_scratch(&scratch);
      // }

    }
  }


  return 0;
}







