#ifndef UI_STACKS_CPP
#define UI_STACKS_CPP

#include "ui_stacks.h"

///////////////////////////////////////////////////////////
// Damian: Text color
//
void ui_push_text_color(Color value)
{
  UI_text_color_node* node = ArenaPush(ui_current_build_arena(), UI_text_color_node);
  node->value = value;
  StackPush(g_ui_state->text_color_stack, node);
  g_ui_state->text_color_stack->count += 1;
}  

void ui_pop_text_color()
{
  if (g_ui_state->text_color_stack->count > 1)
  {
    StackPop(g_ui_state->text_color_stack);
    g_ui_state->text_color_stack->count -= 1;
  }
}

Color ui_current_text_color()
{
  Color value = g_ui_state->text_color_stack->first->value;
  return value;
}

void ui_set_text_color(Color value)
{
  g_ui_state->current_parent->text_color = value;
}

///////////////////////////////////////////////////////////
// Damian: Background color
//
void ui_push_background_color(Color value)
{
  UI_background_color_node* node = ArenaPush(ui_current_build_arena(), UI_background_color_node);
  node->value = value;
  StackPush(g_ui_state->background_color_stack, node);
  g_ui_state->background_color_stack->count += 1;
}

void ui_pop_background_color()
{
  if (g_ui_state->background_color_stack->count > 1)
  {
    StackPop(g_ui_state->background_color_stack);
    g_ui_state->background_color_stack->count -= 1;
  }
}

Color ui_current_backgound_color()
{
  Color value = g_ui_state->background_color_stack->first->value;
  return value;
}

void ui_set_background_color(Color value)
{
  g_ui_state->current_parent->backgound_color = value;
}

///////////////////////////////////////////////////////////
// Damian: Padding
//
void ui_push_padding(F32 value)
{
  UI_padding_node* node = ArenaPush(ui_current_build_arena(), UI_padding_node);
  node->value = value;
  StackPush(g_ui_state->padding_stack, node);
  g_ui_state->padding_stack->count += 1;
}

void ui_pop_padding()
{
  if (g_ui_state->padding_stack->count > 1)
  {
    StackPop(g_ui_state->padding_stack);
    g_ui_state->padding_stack->count -= 1;
  }
}

F32 ui_current_padding()
{
  UI_padding_stack* stack = g_ui_state->padding_stack;
  Assert(stack->count >= 1);
  return stack->first->value;
}

void ui_set_padding(F32 value)
{
  g_ui_state->current_parent->padding = value;
}

///////////////////////////////////////////////////////////
// Damian: Child gap
//
void ui_push_child_gap(F32 value)
{
  UI_child_gap_node* node = ArenaPush(ui_current_build_arena(), UI_child_gap_node);
  node->value = value;
  StackPush(g_ui_state->child_gap_stack, node);
  g_ui_state->child_gap_stack->count += 1;
}

void ui_pop_child_gap()
{
  if (g_ui_state->child_gap_stack->count > 1)
  {
    StackPop(g_ui_state->child_gap_stack);
    g_ui_state->child_gap_stack->count -= 1;
  }
}

F32 ui_current_child_gap()
{
  UI_child_gap_stack* stack = g_ui_state->child_gap_stack;
  Assert(stack->count >= 1);
  return stack->first->value;
} 

void ui_set_child_gap(F32 value)
{
  g_ui_state->current_parent->child_gap = value;
}












#endif