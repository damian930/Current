#ifndef D_UI_H
#define D_UI_H

#include "base/arena.h"
#include "text/text.h"
#include "ui_stacks.h"

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


enum UI_box_flags : U32 {
  UI_box_flag__NONE            = (1 << 0),
  UI_box_flag__has_backgound   = (1 << 1),
  UI_box_flag__has_text        = (1 << 2),
  UI_box_flag__draw_padding    = (1 << 3),
  UI_box_flag__draw_child_gap  = (1 << 4),
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
  Color padding_color;
  Color child_gap_color;
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

// Extra accesors
UI_size ui_size_make(UI_size_kind kind, F32 value);
#define UI_SizePx(v)              ui_size_make(UI_size_kind_px, v)
#define UI_SizeChildrenSum()      ui_size_make(UI_size_kind_children_sum, Null)
#define UI_SizePercentOfParent(p) ui_size_make(UI_size_kind_percent_of_parent, p)
#define UI_SizeText()             ui_size_make(UI_size_kind_text, Null)

Arena* ui_current_build_arena();
Arena* ui_prev_build_arena();

// State
void ui_state_init(Win32_window* window, Font_info* font_info);
void ui_state_release();

// DEBUG Input stuff
UI_Box* ui_get_box_with_key_opt(UI_Box* root, Str8 key);
B32 test_inputs_for_box(UI_Box* box);
B32 ui_is_clicked();

// UI element creation stuff
UI_Box* ui_allocate_box_helper(Arena* arena, 
                               UI_size size_kind_x, UI_size size_kind_y, Axis2 alignment_axis, 
                               const char* key, UI_box_flags flags, 
                               Color backgound_color,const char* text);
void ui_begin_build();
void ui_end_build();
UI_Box* ui_begin_box(UI_size size_kind_x, UI_size size_kind_y, Axis2 alignment_axis, 
                     const char* key, UI_box_flags flags,
                     Color color, const char* c_str);
void ui_end_box();

// UI sizing/layout/pos stuff
void ui_sizing_for_fixed_sized_elements(UI_Box* root, Axis2 axis);
void ui_sizing_for_child_dependant_elements(UI_Box* root, Axis2 axis);
void ui_sizing_for_parent_dependant_elements(UI_Box* root, Axis2 axis);
void ui_layout_pass(UI_Box* root, Axis2 axis);
void ui_final_pos_pass(UI_Box* root);

// Draw
void ui_draw_ui_helper(UI_Box* root);
void ui_draw_ui();

// THESE ARE JUST SOME EXTRA THINGS I AM TESTING AND DONT WANT TO PUT IN OTHER FUNCS FOR NOW
void ui_draw_padding_for_current(Color padding_color);
void ui_draw_child_gap_color(Color gap_color);

///////////////////////////////////////////////////////////
// Damian: TODO stuff
//
// [] Figure out a place where to calculate padding 
// [x] Padding coloring
// [x] Chlld gap coloring
// [] Border     
// [] Border coloring
// [] Element that fits the parent
//    [] NOTE 1*
// =======================================================
// NOTE*: 
//  Test elements that are fit parent, they would be parent dependant,
//  therefore calculated before child_sum elements. This means why wont extend att all. 
//  Maybe a nice behaviour for it would be to have then fit the parent, so if parent is in px,
//  then this elements just stretched to its max possible size to then fully use up the parent space.
//  This wont do anything for child sum, so maybe then, child sum shoud be ignored in this case.
//  So if fit parent is used inside child sum, it will be promoted to the nearest parent who is now child_sum,
//  then we get the fitting size from that element, and since all its children until the fit elements 
//  are child_sum kinds, those can be extended. So we kinda just go over the childsum and fit the first
//  "fittable" parent (the one whos size is not child depandant)


#endif