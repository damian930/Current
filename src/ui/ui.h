#ifndef D_UI_H
#define D_UI_H

#include "base/arena.h"
#include "text/text.h"

enum Axis2 {
  Axis2_x,
  Axis2_y,
  Axis2_COUNT,
};

enum UI_size_kind {
  UI_size_kind_px,
  UI_size_kind_children_sum,
  UI_size_kind_percent_of_parent,
  UI_size_kind_text,
};

struct UI_size {
  UI_size_kind kind;
  F32 value;
};

// Padding stack stuff
struct UI_padding_node {
  UI_padding_node* next;
  F32 value;
};

struct UI_padding_stack {
  UI_padding_node* first;
  U32 count;
};
// ======================

// Child gap stuff
struct UI_child_gap_node {
  UI_child_gap_node* next;
  F32 value;
};

struct UI_child_gap_stack {
  UI_child_gap_node* first;
  U32 count;
};

// ======================

enum UI_box_flags : U32 {
  UI_box_flag__NONE          = (1 << 0),
  UI_box_flag__has_backgound = (1 << 1),
  UI_box_flag__has_text      = (1 << 2),
};

struct UI_Box {
  UI_Box* parent;
  UI_Box* first;
  UI_Box* last;
  UI_Box* next;
  UI_Box* prev;
  U32 children_count;

  // Nature of a box
  Str8 key;
  UI_size semantic_size[Axis2_COUNT];
  Axis2 alignment_axis;

  // Extra features
  UI_box_flags flags;
  Color backgound_color;
  Str8 text;

  // Sizing pass data
  F32 computed_sizes[Axis2_COUNT];
  F32 computed_parent_rel_pos[Axis2_COUNT];
  Rect computed_final_rect;
};

struct UI_state {
  Arena* perm_state_arena;

  U8 current_arena_index;
  Arena* ui_tree_build_arenas[2];

  Win32_window* window;
  Font_info* font_info;
  Texture2D font_texture;

  UI_Box* root;
  UI_Box* current_parent; // NOTE: This name might be confusing, may have to change it 

  // Prev frame tree
  UI_Box* prev_frame_root;

  UI_padding_stack* padding_stack;
  UI_child_gap_stack* child_gap_stack;
};

global UI_state* g_ui_state = 0;

// void ui_state_init();
// void ui_state_release();

// void ui_begin_build();
// void ui_end_build();







#endif