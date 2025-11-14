#ifndef D_UI_H
#define D_UI_H

#include "base/arena.h"
#include "font/font.h"
#include "ui_stacks.h"

enum Axis2 {
  Axis2_x,
  Axis2_y,
  Axis2_COUNT,
};
// TODO: move this
Axis2 axis2_other(Axis2 axis)
{
  Axis2 result = (axis == Axis2_x ? Axis2_y : Axis2_x);
  return result;
}

enum UI_size_kind {
  UI_size_kind_px,
  UI_size_kind_children_sum,
  UI_size_kind_text,
  UI_size_kind_percent_of_parent,
  UI_size_kind_fit_the_parent,
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

// TESTING STUFF IN HERE
struct UI_Inputs {
  B32 is_hovered;
  B32 is_pressed_left;
  B32 is_clicked;
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
  Str8 text;
  Color text_color;
  Color backgound_color;
  Color padding_color;
  Color child_gap_color;
  
  F32 padding;
  F32 child_gap;

  // B32 has_min_size[Axis2_COUNT];
  // B32 has_max_size[Axis2_COUNT];
  // F32 min_size[Axis2_COUNT];
  // F32 max_size[Axis2_COUNT];

  // Sizing pass data
  F32 computed_sizes[Axis2_COUNT];
  F32 computed_parent_rel_pos[Axis2_COUNT];
  Rect computed_final_rect;

  UI_Inputs inputs;
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

  UI_text_color_stack* text_color_stack;
  UI_background_color_stack* background_color_stack;
  UI_padding_stack* padding_stack;
  UI_child_gap_stack* child_gap_stack;
};

global UI_state* g_ui_state = 0;

// Extra accesors
UI_size ui_size_make(UI_size_kind kind, F32 value);
#define UI_SizePx(v)              ui_size_make(UI_size_kind_px, v)
#define UI_SizeChildrenSum()      ui_size_make(UI_size_kind_children_sum, Null)
#define UI_SizeText()             ui_size_make(UI_size_kind_text, Null)
#define UI_SizePercentOfParent(p) ui_size_make(UI_size_kind_percent_of_parent, p)
#define UI_SizeFitTheParent()     ui_size_make(UI_size_kind_fit_the_parent, Null)
Arena* ui_current_build_arena();
Arena* ui_prev_build_arena();

// State
void ui_state_init(Win32_window* window, Font_info* font_info);
void ui_state_release();

// DEBUG Input stuff


// UI_Box* ui_get_box_with_key_opt(UI_Box* root, Str8 key);
// B32 test_inputs_for_box(UI_Box* box);
// B32 ui_is_clicked();



// UI element creation stuff
UI_Box* ui_allocate_box_helper(Arena* arena, 
                               UI_size size_kind_x, UI_size size_kind_y, Axis2 alignment_axis, 
                               const char* key, UI_box_flags flags, 
                               const char* text);
void ui_begin_build();
void ui_end_build();
UI_Box* ui_begin_box(UI_size size_kind_x, UI_size size_kind_y, Axis2 alignment_axis, 
                     const char* key, UI_box_flags flags,
                     const char* c_str);
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
/*
[ ] Figure out a place where to calculate padding 
[x] Padding coloring
[x] Chlld gap coloring
[ ] Border     
[ ] Border coloring
[ ] Element that fits the parent
   [ ] NOTE 1*
=======================================================
NOTE 1*: 
  I am now implementing a fit the parent idea. I decided it to be a separate size kind from % of parent.
  This makes sense since % of parent is still a px size (kind of) and if is also specified up front by the builder code for the ui.
  The fit aspect of an element like spacer is only calculated after all the other sizes are done to somply fill in the space.
  This is kinda weird to have a separate type for only spacers (i cant thing of other placer where i would need it), but
  i dont want to have some weird flags on % of parent than then tell the core code to delay the sizing.
  I am going to have a separate type for this, and if it doesnt work or can be simplifies, then great.
  Idea is the following:
    - We do the static sizing for text and px.
    - We do the children sum 
    - Then we go the % of parent (we get the parent of parent of our parent of child_sum kind sized element)
    - Now we have all the sizes, so we size all the fit the parent elements.
      This step will work with fit_kind, %_of_parent kind and child_sum_kind element.
      The % of parent will extend the child_sum_elements. 
      The fit will also get the parent of parent (the first hardcoded size), get all the space left, fill it in.
      If the parent of the fit element is child_sum, then this child_sum will have to be extended with the new filler size.
      Therefore in the end, we end up with a flex box kind of thing i guess.
 */


#endif