#ifndef UI_STACKS_H
#define UI_STACKS_H

#include "ui.h"

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