#ifndef D_UI_WIDGETS_H
#define D_UI_WIDGETS_H

#include "ui_core.h"

// UI_Inputs ui_label(Str8 key, Str8 str)
// {
//   UI_Inputs inputs = {};
//   UI_SizeX(ui_size_text_make(0.5))
//   UI_SizeY(ui_size_text_make(0.5))
//   {
//     inputs = ui_box_make(key, UI_box_flag__has_text|UI_box_flag__has_backgound, str);
//   }
//   return inputs;
// }

// UI_Inputs ui_label(const char* key, Str8 str)
// {
//   Scratch scratch = get_scratch();
//   Str8 key_str8 = str8_from_cstr(key);
//   UI_Inputs inputs = ui_label(key_str8, str);
//   end_scratch(&scratch);
//   return inputs;
// }

// void ui_spacer(Arena* arena, Axis2 axis)
// {
//   UnusedVar(arena);

//   if (axis == Axis2_x)
//   {
//     DefereLoop(ui_push_size_x(ui_size_fit_the_parent_make(1)), ui_pop_size_x())
//     DefereLoop(ui_push_size_y(ui_size_px_make(0.0f)), ui_pop_size_y())
//     // DefereLoop(ui_push_size_y(ui_size_px_make(5.0f)), ui_pop_size_y())
//     // DefereLoop(ui_push_background_color(C_YELLOW), ui_pop_background_color()) // This is here for now to see the spacer working properly
//     {
//       ui_box_make(Str8FromClit("spacer"), UI_box_flag__has_backgound|UI_box_flag__NONE, str8_empty());
//     }
//   }
//   else if (axis == Axis2_y)
//   {
//     DefereLoop(ui_push_size_x(ui_size_px_make(5.0f)), ui_pop_size_x())
//     DefereLoop(ui_push_size_y(ui_size_fit_the_parent_make(1)), ui_pop_size_y())
//     DefereLoop(ui_push_background_color(C_YELLOW), ui_pop_background_color()) // This is here for now to see the spacer working properly
//     {
//       ui_box_make(str8_empty(), UI_box_flag__has_backgound|UI_box_flag__NONE, str8_empty());
//     }
//   }
//   else { InvalidCodePath(); }
// }

// void ui_image_pro(Str8 key, Texture2D texture, F32 width, F32 height)
// {
//   DefereLoop(ui_push_size_x(ui_size_px_make(width)), ui_pop_size_x())
//   DefereLoop(ui_push_size_y(ui_size_px_make(height)), ui_pop_size_y())
//   {
//     UI_BoxLoop(key, UI_box_flag__NONE, str8_empty())
//     {
//       ui_set_texture(texture);
//     }
//   }
// }

// void ui_image(Str8 key, Texture2D texture)
// {
//   ui_image_pro(key, texture, (F32)texture.width, (F32)texture.height);
// }

void ui_slider(Str8 key, F32* value, F32 min_value, F32 max_value)
{
  UI_Inputs inputs = {};

  UI_SizeX(ui_size_px_make(100))
  UI_SizeY(ui_size_px_make(30))
  UI_LayoutAxis(Axis2_x)
  UI_BackgroundColor(C_BLUE)
  UI_BoxLoop(key, UI_box_flag__has_backgound)
  {
    F32 min_value_adj = min_value - min_value;
    F32 max_value_adj = max_value - min_value;
    F32 value_p = *value / max_value_adj;

    UI_SizeX(ui_size_fit_the_parent_make(value_p))
    UI_SizeY(ui_size_px_make(0))
    UI_BoxLoop(ui_null_key(), UI_box_flag__NONE)
    {}

    UI_SizeX(ui_size_px_make(10))
    UI_SizeY(ui_size_px_make(100))
    UI_BoxLoop(ui_null_key(), UI_box_flag__has_backgound)
    {
      ui_set_background_color(C_GREY);
    }

    UI_SizeX(ui_size_fit_the_parent_make(1.0f - value_p))
    UI_SizeY(ui_size_px_make(0))
    UI_BoxLoop(ui_null_key(), UI_box_flag__NONE)
    {}

    inputs = ui_get_inputs_from_prev_build();
  }

  if (inputs.is_pressed_left)
  {
    Vec2 mouse_pos = inputs.withing_widget_mouse_pos;
    F32 mouse_pos_p_in_box = mouse_pos.x / 100.0f; // TODO: Here shoud be the width of the slider box
    *value = mouse_pos_p_in_box * max_value;
    // *value = clamp_f32(*value, min_value, max_value);
  }

}











#endif