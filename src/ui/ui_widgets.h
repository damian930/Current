#ifndef D_UI_WIDGETS_H
#define D_UI_WIDGETS_H

#include "ui_core.h"
#include "ui_core.cpp"

void ui_label(const char* str, const char* key)
{
  ui_begin_box(UI_SizeText(), UI_SizeText(), Axis2_x, key, UI_box_flag__has_text, C_TRANSPARENT, str);
  ui_end_box();
}

B32 ui_button(Color color, const char* text, const char* key, const char* text_key)
{
  B32 is_pressed = false;
  ui_begin_box(UI_SizeChildrenSum(), UI_SizeChildrenSum(), Axis2_x, key, UI_box_flag__has_backgound, color, key);
  {
    ui_label(text, text_key);
    is_pressed = ui_is_clicked();
  }
  ui_end_box();
  return is_pressed;
}





















#endif