#ifndef D_UI_WIDGETS_H
#define D_UI_WIDGETS_H

#include "ui_core.h"

UI_Inputs ui_label(Str8 key, Str8 str)
{
  UI_Inputs inputs = {};
  DefereLoop(ui_push_size_x(ui_size_text_make()), ui_pop_size_x())
  DefereLoop(ui_push_size_y(ui_size_text_make()), ui_pop_size_y())
  {
    inputs = ui_box_make(key, UI_box_flag__has_text|UI_box_flag__has_backgound, str);
  }
  return inputs;
}

void ui_spacer(Axis2 axis)
{
  if (axis == Axis2_x)
  {
    DefereLoop(ui_push_size_x(ui_size_fit_the_parent_make()), ui_pop_size_x())
    DefereLoop(ui_push_size_y(ui_size_px_make(5.0f)), ui_pop_size_y())
    DefereLoop(ui_push_background_color(C_YELLOW), ui_pop_background_color()) // This is here for now to see the spacer working properly
    {
      ui_box_make(str8_empty(), UI_box_flag__has_backgound|UI_box_flag__NONE, str8_empty());
    }
  }
  else if (axis == Axis2_y)
  {
    DefereLoop(ui_push_size_x(ui_size_px_make(5.0f)), ui_pop_size_x())
    DefereLoop(ui_push_size_y(ui_size_fit_the_parent_make()), ui_pop_size_y())
    DefereLoop(ui_push_background_color(C_YELLOW), ui_pop_background_color()) // This is here for now to see the spacer working properly
    {
      ui_box_make(str8_empty(), UI_box_flag__has_backgound|UI_box_flag__NONE, str8_empty());
    }
  }
  else { InvalidCodePath(); }
}


















#endif