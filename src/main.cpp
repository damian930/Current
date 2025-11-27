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

#include "process_tracker/process_tracker.h"
#include "process_tracker/process_tracker.cpp"

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

#include "json/json.h"
#include "json/json.cpp"
void EntryPoint()
{
  Arena* arena = arena_alloc(Kilobytes_U64(64), "Json arena");
  
  DefereInitReleaseLoop(Scratch scratch = get_scratch(0, 0), end_scratch(&scratch))
  {
    Data_buffer file_buffer = os_win32_file_read_inplace(scratch.arena, Str8FromClit("../src/json/json_test.json"));
    Json_value* ast = test_create_ast_for_json(arena, file_buffer);
    Str8 json_as_str = json_debug_recreate_json(arena, ast);
    str8_printf("#Str8 \n", json_as_str); 
  }

}

// -----

#if 0

UI_Inputs pt_button(Str8 key)
{
  UI_Inputs inputs = {};
  UI_SizeX(ui_size_child_sum_make())
  UI_SizeY(ui_size_child_sum_make())
  UI_Padding(15)
  UI_LayoutAxis(Axis2_x)
  UI_BoxLoop(key, UI_box_flag__has_backgound|UI_box_flag__clickable)
  {
    inputs = ui_get_inputs_from_prev_build();

    Color background_color = color_normalise(44, 46, 49, 255);
    if (inputs.is_hovered) { 
      background_color.r += 0.075f;
      background_color.g += 0.075f;
      background_color.b += 0.075f;
    }
    ui_set_background_color(background_color);
    ui_set_text_color(C_WHITE);

    UI_SizeX(ui_size_text_make())
    UI_SizeY(ui_size_text_make())
    UI_BoxLoop(key, UI_box_flag__has_text) {}
  }
  
  return inputs;
}

void pt_label(Str8 text)
{
  UI_SizeX(ui_size_text_make())
  UI_SizeY(ui_size_text_make())
  UI_Padding(0)
  UI_BoxLoop(text, UI_box_flag__has_text|UI_box_flag__has_backgound|UI_box_flag__draw_text_box) {}
}

void pt_menu()
{
  UI_SizeX(ui_size_child_sum_make())
  UI_SizeY(ui_size_fit_the_parent_make(1.0f))
  UI_Padding(10)
  UI_ChildGap(10)
  UI_BoxLoop(Str8FromClit("Pt_menu"), UI_box_flag__has_backgound)
  {
    ui_set_background_color(C_LIGHT_GREEN);
    UI_Inputs inputs = pt_button(Str8FromClit("Show processes##1"));
    if (inputs.is_clicked)
    {
      str8_printf("Button clicked \n");
    }
  }
}


void pt_data_part()
{
  UI_LayoutAxis(Axis2_y)
  UI_SizeX(ui_size_fit_the_parent_make(1))
  UI_SizeY(ui_size_fit_the_parent_make(1))
  UI_Padding(10)
  UI_ChildGap(10)
  UI_BoxLoop(Str8FromClit("Data part vstack"), UI_box_flag__has_backgound)
  {
    ui_set_background_color(C_BROWN);
  
    // Row for the names of the stuff
    UI_SizeX(ui_size_fit_the_parent_make(1))
    UI_SizeY(ui_size_child_sum_make())
    UI_LayoutAxis(Axis2_x)
    UI_BoxLoop(Str8FromClit("Data kinds row"), UI_box_flag__has_backgound)
    {
      ui_set_background_color(C_BLUE);
      pt_label(Str8FromClit("Name##"));
      pt_label(Str8FromClit("Start Time##"));
      pt_label(Str8FromClit("Elapsed Time##"));
    }
    
    // Rows for process data
    UI_SizeX(ui_size_fit_the_parent_make(1))
    UI_SizeY(ui_size_child_sum_make())
    UI_LayoutAxis(Axis2_y)
    UI_BoxLoop(Str8FromClit("Data kinds row"), UI_box_flag__has_backgound)
    {
      Arena* fake_scratch_arena = arena_alloc(Kilobytes_U64(60), "Fake scratch");
      {
        Process_data_list* list = get_all_process_data(fake_scratch_arena);
        for (Process_data_node* node = list->first; node != 0; node = node->next)
        {
          if (node->process_data.path.count > 0)
          {
            UI_SizeX(ui_size_fit_the_parent_make(1))
            UI_SizeY(ui_size_child_sum_make())
            UI_LayoutAxis(Axis2_x)
            UI_BoxLoop(Str8FromClit("123123"), UI_box_flag__has_backgound|UI_box_flag__draw_child_gap)
            {
              ui_set_child_gap(40);
              ui_set_child_gap_color(C_RED);
              ui_set_background_color(C_BLUE);
              ui_set_padding(0);
              
              Process_data* data = &node->process_data;
              {
                Time current_local_time = os_win32_get_current_time_utc();

                UI_BackgroundColor(C_LIGHT_GREEN)
                {
                  pt_label(data->path);
                }

                UI_BackgroundColor(C_GREEN)
                UI_SizeX(ui_size_fit_the_parent_make(1))
                UI_SizeY(ui_size_fit_the_parent_make(1))
                UI_BoxLoop(Str8FromClit("Spacer test"), UI_box_flag__has_backgound) {}
                
                Str8 time = time_as_str8(fake_scratch_arena, data->creation_time);
                pt_label(time);
                
                UI_BackgroundColor(C_GREEN)
                UI_SizeX(ui_size_fit_the_parent_make(1))
                UI_SizeY(ui_size_fit_the_parent_make(1))
                UI_BoxLoop(ui_null_key(), UI_box_flag__has_backgound) {}

                pt_label(time_as_str8(fake_scratch_arena, current_local_time));
              }
            }
            
            UI_SizeX(ui_size_fit_the_parent_make(1))
            UI_SizeY(ui_size_px_make(1))
            UI_BackgroundColor(C_BLACK)
            UI_BoxLoop(ui_null_key(), UI_box_flag__has_backgound) {}
          }
            
        }
      }
      arena_release(fake_scratch_arena); 
    }
  }
}

void EntryPoint()
{
  Arena* font_arena = arena_alloc(Megabytes_U64(10), "Font test arena");
  // #define FONT_PATH "../data/papyrus.ttf"
  #define FONT_PATH "../data/Roboto-Regular.ttf"
  Font_info* font_info = load_font(font_arena, range_u32(' ' + 1, '~'), 32, Str8FromClit(FONT_PATH));

  Arena* process_arena = arena_alloc(Megabytes_U64(10), "Process arena");
  Process_data_list* list = get_all_process_data(process_arena);
  // for (Process_data_node* node = list->first; node != 0; node = node->next)
  // {
  //   Str8 path = str8_temp_from_str8(node->process_data.path); //str8_temp_from_str8(node->process_data.path);
  //   printf("Path: %s \n", path.data);
  // }

  Arena* image_arena = arena_alloc(Megabytes_U64(30), "Image test arena");
  Image2D jimmy_image = load_png(image_arena, Str8FromClit("../data/jimmy.png"), false);
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
              ui_set_background_color(C_GREY);
             
              UI_LayoutAxis(Axis2_x)
              UI_BoxLoop(Str8FromClit("Main"), UI_box_flag__NONE)
              {
                ui_set_size_x(ui_size_fit_the_parent_make(1));
                ui_set_size_y(ui_size_fit_the_parent_make(1));
                
                UI_LayoutAxis(Axis2_x)
                UI_SizeX(ui_size_fit_the_parent_make(1))
                UI_SizeY(ui_size_fit_the_parent_make(1))
                UI_Padding(50)
                UI_ChildGap(10)
                UI_BoxLoop(Str8FromClit("Data part vstack"), UI_box_flag__has_backgound)
                {
                  ui_set_background_color(C_BROWN);
                  ui_draw_padding_for_current(C_LIGHT_GREEN);

                  pt_label(Str8FromClit("Some text"));

                  UI_SizeX(ui_size_fit_the_parent_make(1))
                  UI_SizeY(ui_size_px_make(1))
                  UI_BackgroundColor(C_BLUE)
                  UI_BoxLoop(Str8FromClit("Spacer"), UI_box_flag__has_backgound) {}

                  pt_label(Str8FromClit("Some text"));

                  UI_SizeX(ui_size_fit_the_parent_make(0.3f))
                  UI_SizeY(ui_size_px_make(1))
                  UI_BackgroundColor(C_BLUE)
                  UI_BoxLoop(Str8FromClit("Spacer"), UI_box_flag__has_backgound|UI_box_flag__text_colapse) {}

                  pt_label(Str8FromClit("S"));

                  UI_SizeX(ui_size_fit_the_parent_make(1))
                  UI_SizeY(ui_size_px_make(1))
                  UI_BackgroundColor(C_BLUE)
                  UI_BoxLoop(Str8FromClit("Spacer"), UI_box_flag__has_backgound) {}

                  pt_label(Str8FromClit("Other text"));
                }

                // pt_menu();
                // pt_data_part();
              }
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



#endif 




