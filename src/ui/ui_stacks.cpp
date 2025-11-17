#ifndef UI_STACKS_CPP
#define UI_STACKS_CPP

#include "ui_stacks.h"

// Defining stack push functions
#define UI_STACK_DATA(stack_struct_name,     \
                      stack_var_name,        \
                      node_struct_name,      \
                      node_var_name,         \
                      Value_type,            \
                      value_var_name,        \
                      push_func_name,        \
                      pop_func_name,         \
                      get_current_func_name) \
  void push_func_name(Value_type value)      \
  {                                          \
    node_struct_name* new_node = ArenaPush(ui_current_build_arena(), node_struct_name); \
    new_node->value_var_name = value;                                                   \
    StackPush(g_ui_state->stack_var_name, new_node);                                    \
    g_ui_state->stack_var_name->count += 1;                                             \
  }
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA

// Defining stack pop functions
#define UI_STACK_DATA(stack_struct_name,     \
                      stack_var_name,        \
                      node_struct_name,      \
                      node_var_name,         \
                      Value_type,            \
                      value_var_name,        \
                      push_func_name,        \
                      pop_func_name,         \
                      get_current_func_name) \
  void pop_func_name()                       \
  {                                          \
    StackPop(g_ui_state->stack_var_name);    \
    Assert(g_ui_state->stack_var_name->count >= 1); \
  }
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA

// Defining stack get_current functions
#define UI_STACK_DATA(stack_struct_name,     \
                      stack_var_name,        \
                      node_struct_name,      \
                      node_var_name,         \
                      Value_type,            \
                      value_var_name,        \
                      push_func_name,        \
                      pop_func_name,         \
                      get_current_func_name) \
  Value_type get_current_func_name()         \
  {                                          \
    Value_type value = g_ui_state->stack_var_name->first->value; \
    return value;                                                \
  }
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA









#endif