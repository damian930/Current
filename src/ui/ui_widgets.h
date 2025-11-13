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

void ui_checkbox_entry(B32* b, U32* value, U32 value_on_pick, const char* text, const char* key)
{
  // Box, label
  Color color = (*b ? C_LIGHT_GREEN : C_BLUE);
  ui_begin_box(UI_SizePx(30), UI_SizePx(30), Axis2_x, key, UI_box_flag__has_backgound, color, "");
  {
    if (ui_is_clicked())
    {
      ToggleBool(*b);
    }
    if (*b)
    {
      *value = value_on_pick;
    }
  }
  ui_end_box();
}

void ui_spacer()
{
  ui_begin_box(UI_SizeFitTheParent(), UI_SizeChildrenSum(), Axis2_x, 
              "Fit 1", UI_box_flag__NONE, C_LIGHT_GREEN, "");
  ui_end_box();
}

#define ui_h_stack(key) DefereLoop(ui_begin_box(UI_SizeFitTheParent(), UI_SizeChildrenSum(), \
                                                Axis2_x, \
                                                key, UI_box_flag__NONE, C_TRANSPARENT, ""), \
                                   ui_end_box())

#define ui_v_stack(key) DefereLoop(ui_begin_box(UI_SizeChildrenSum(), UI_SizeFitTheParent(), \
                                                Axis2_y, \
                                                key, UI_box_flag__NONE, C_TRANSPARENT, ""), \
                                   ui_end_box())


















#endif