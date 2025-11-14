#ifndef UI_STACKS_H
#define UI_STACKS_H

#include "ui_core.h"

// TODO: Add headers for push pop get for stacks

// Testing to see how macros would work for this
// #define UI_DeclareNode(Node_type_name, Value_type) struct Node_type_name { Node_type_name* next; Value_type value; };  //UI_DeclareNode_Helper(UI_ConstructNodeName(node_name), Value_type) 
// #define UI_DeclareStack(Stack_type_name, Node_type_name) struct Stack_type_name { Node_type_name* first; U32 count; };  //UI_DeclareStack_Helper(UI_ConstructStackName(stack_name), UI_node_type) 

// #define UI_DefineStack_Push(func_name, stack_name, Node_type_name, Value_type) \
//   void func_name(Value_type new_value) \
//   { \
//     Node_type_name* node = ArenaPush(ui_current_build_arena(), Node_type_name); \
//     node->value = new_value; \
//     StackPush(g_ui_state->stack_name, node); \
//     g_ui_state->stack_name->count += 1; \
//   }; 

// UI_DeclareNode(UI_background_color_node, Color);
// UI_DeclareStack(UI_background_color_stack, UI_background_color_node);

// UI_DefineStack_Push(ui_push_backgound_color, background_color_stack, UI_background_color_node, Color);

///////////////////////////////////////////////////////////
// Damian: Text color
//
struct UI_text_color_node {
  UI_text_color_node* next;
  Color value;
};

struct UI_text_color_stack {
  UI_text_color_node* first;
  U32 count;
};

///////////////////////////////////////////////////////////
// Damian: Backgound color
//
struct UI_background_color_node {
  UI_background_color_node* next;
  Color value;
}; 

struct UI_background_color_stack {
  UI_background_color_node* first;
  U32 count;
};

///////////////////////////////////////////////////////////
// Damian: Padding
//
struct UI_padding_node {
  UI_padding_node* next;
  F32 value;
};

struct UI_padding_stack {
  UI_padding_node* first;
  U32 count;
};

///////////////////////////////////////////////////////////
// Damian: Child gap
//
struct UI_child_gap_node {
  UI_child_gap_node* next;
  F32 value;
};

struct UI_child_gap_stack {
  UI_child_gap_node* first;
  U32 count;
};








#endif