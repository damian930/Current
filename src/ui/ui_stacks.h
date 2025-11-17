#ifndef UI_STACKS_H
#define UI_STACKS_H

#include "ui_core.h"

// Table for the ui stacks 
#define UI_STACK_DATA_TABLE \
  UI_STACK_DATA(UI_background_color_stack, background_color_stack, UI_background_node, node, Color, value, ui_push_background_color, ui_pop_background_color, ui_current_backgound_color) \
  UI_STACK_DATA(UI_child_gap_stack,        child_gap_stack,        UI_child_gap_node,  node, F32,   value, ui_push_child_gap,        ui_pop_child_gap,        ui_current_child_gap      ) \
  UI_STACK_DATA(UI_text_color_stack,       text_color_stack,       UI_text_color_node, node, Color, value, ui_push_text_color,       ui_pop_text_color,       ui_current_text_color     ) \
  UI_STACK_DATA(UI_padding_stack,          padding_stack,          UI_padding_node,    node, F32,   value, ui_push_padding,          ui_pop_padding,          ui_current_padding        ) 

// Declaring stacks here
#define UI_STACK_DATA(stack_struct_name,     \
                      stack_var_name,        \
                      node_struct_name,      \
                      node_var_name,         \
                      Value_type,            \
                      value_var_name,        \
                      push_func_name,        \
                      pop_func_name,         \
                      get_current_func_name) \
  struct node_struct_name {                  \
    node_struct_name* next;                  \
    Value_type value_var_name;               \
  };                                         \
  struct stack_struct_name {                 \
    node_struct_name* first;                 \
    U32 count;                               \
  }; 
  UI_STACK_DATA_TABLE;
#undef UI_STACK_DATA

// Declaring stack push/pop/get_current functions
#define UI_STACK_DATA(stack_struct_name,     \
                      stack_var_name,        \
                      node_struct_name,      \
                      node_var_name,         \
                      Value_type,            \
                      value_var_name,        \
                      push_func_name,        \
                      pop_func_name,         \
                      get_current_func_name) \
  void push_func_name();                     \
  void pop_func_name();                      \
  Value_type get_current_func_name();        
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA








#endif