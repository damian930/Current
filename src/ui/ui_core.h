#ifndef D_UI_H
#define D_UI_H

#include "base/include.h"
#include "font/font.h"

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
};

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

// TODO: I wanted this to be in some other spot, but i just cant make it compile
///////////////////////////////////////////////////////////
// Damian: Stacks
//
#define UI_STACK_DATA_TABLE \
  UI_STACK_DATA( UI_background_color_stack, background_color_stack, UI_background_node,      node, Color,   value, C_TRANSPARENT,         ui_push_background_color, ui_pop_background_color, ui_current_backgound_color ) \
  UI_STACK_DATA( UI_child_gap_stack,        child_gap_stack,        UI_child_gap_node,       node, F32,     value, 0.0f,                  ui_push_child_gap,        ui_pop_child_gap,        ui_current_child_gap       ) \
  UI_STACK_DATA( UI_child_gap_color_stack,  child_gap_color_stack,  UI_child_gap_color_node, node, Color,   value, C_TRANSPARENT,         ui_push_child_gap_color,  ui_pop_child_gap_color,  ui_current_child_gap_color ) \
  UI_STACK_DATA( UI_text_color_stack,       text_color_stack,       UI_text_color_node,      node, Color,   value, C_WHITE,               ui_push_text_color,       ui_pop_text_color,       ui_current_text_color      ) \
  UI_STACK_DATA( UI_padding_stack,          padding_stack,          UI_padding_node,         node, F32,     value, 0.0f,                  ui_push_padding,          ui_pop_padding,          ui_current_padding         ) \
  UI_STACK_DATA( UI_padding_color_stack,    padding_color_stack,    UI_padding_color_node,   node, Color,   value, C_TRANSPARENT,         ui_push_padding_color,    ui_pop_padding_color,    ui_current_padding_color   ) \
  UI_STACK_DATA( UI_size_x_stack,           size_x_stack,           UI_size_x_node,          node, UI_size, value, ui_size_px_make(0.0f), ui_push_size_x,           ui_pop_size_x,           ui_current_size_x          ) \
  UI_STACK_DATA( UI_size_y_stack,           size_y_stack,           UI_size_y_node,          node, UI_size, value, ui_size_px_make(0.0f), ui_push_size_y,           ui_pop_size_y,           ui_current_size_y          ) \
  UI_STACK_DATA( UI_layout_axis_stack,      layout_axis_stack,      UI_layout_axis_node,     node, Axis2,   value, Axis2_y,               ui_push_layout_axis,      ui_pop_layout_axis,      ui_current_layout_axis     )                                                   

// Declaring stacks 
#define UI_STACK_DATA(stack_struct_name, stack_var_name,                    \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
  struct node_struct_name {                                                 \
    node_struct_name* next;                                                 \
    Value_type value_var_name;                                              \
  };                                                                        \
  struct stack_struct_name {                                                \
    node_struct_name* first;                                                \
    U32 count;                                                              \
  }; 
  UI_STACK_DATA_TABLE;
#undef UI_STACK_DATA
//-///////////////////////////////////////////////////////////

enum UI_box_flag : U32 {
  UI_box_flag__NONE            = (1 << 0),
  // Regular features
  UI_box_flag__has_backgound   = (1 << 1),
  UI_box_flag__has_text        = (1 << 2),
  UI_box_flag__has_texture     = (1 << 3),
  // Interactions
  UI_box_flag__clickable       = (1 << 4),
  // Extra drawings
  UI_box_flag__draw_padding    = (1 << 5),
  UI_box_flag__draw_child_gap  = (1 << 6),
};
typedef U32 UI_box_flags;

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
  Axis2 layout_axis;

  // Extra features
  UI_box_flags flags;
  Str8 text;
  Texture2D texture;
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
  B32 is_size_computed;
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

  // Adding stacks
  #define UI_STACK_DATA(stack_struct_name, stack_var_name,                  \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
  stack_struct_name* stack_var_name;
  UI_STACK_DATA_TABLE
  #undef UI_STACK_DATA
};

global UI_state* g_ui_state = 0;

// TODO: Remove this, this is here for debug now
void ui_set_texture(Texture2D texture)
{
  UI_Box* root = g_ui_state->current_parent;
  root->flags |= UI_box_flag__has_texture;
  root->texture = texture;
}
// ---------------------------------------------

// Extra accesors
UI_size ui_size_make(UI_size_kind kind, F32 value);
UI_size ui_size_px_make(F32 value);
UI_size ui_size_child_sum_make();
UI_size ui_size_text_make();
UI_size ui_size_percent_of_parent_make(F32 p);
UI_size ui_size_fit_the_parent_make();
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

// Getters
Arena* ui_current_build_arena();  
Arena* ui_prev_build_arena();

// UI_Box* ui_get_box_with_key_opt(UI_Box* root, Str8 key);
// B32 test_inputs_for_box(UI_Box* box);
// B32 ui_is_clicked();

// UI element creation stuff
// TODO: Either remove 1 of these or make a parameter specific (cstr/str8) name
UI_Box* ui_allocated_and_set_up_box(Str8 key, UI_box_flags flags, Str8 text);
UI_Box* ui_allocated_and_set_up_box(Str8 key, UI_box_flags flags, Str8 text);

void ui_begin_build();
void ui_end_build();

UI_Box* ui_begin_box(Str8 key, UI_box_flags flags, Str8 text);
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

UI_Inputs ui_box_make(Str8 key, UI_box_flags flags, Str8 text);

// Stacks
#define UI_STACK_DATA(stack_struct_name, stack_var_name,                    \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
void push_func_name(Value_type value);       \
void pop_func_name();                        \
Value_type get_current_func_name();        
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA

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