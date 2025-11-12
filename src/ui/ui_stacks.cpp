#ifndef UI_STACKS_CPP
#define UI_STACKS_CPP

#include "ui_stacks.h"

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
  if (g_ui_state->padding_stack->count > 2)
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
  if (g_ui_state->child_gap_stack->count > 2)
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














#endif