#ifndef D_UI_CPP
#define D_UI_CPP

#include "ui.h"
#include "base/arena.cpp"

#include "os/core/os_core_win32.h"

// ---
UI_size ui_size_make(UI_size_kind kind, F32 value)
{
  UI_size result = {};
  result.kind = kind;
  result.value = value;
  return result;
}
#define UI_SizePx(v) ui_size_make(UI_size_kind_px, v)
#define UI_SizeChildrenSum() ui_size_make(UI_size_kind_children_sum, Null)
#define UI_SizePercentOfParent(p) ui_size_make(UI_size_kind_percent_of_parent, p)
#define UI_SizeText() ui_size_make(UI_size_kind_text, Null)

// ---

Arena* ui_current_build_arena()
{
  return g_ui_state->ui_tree_build_arenas[g_ui_state->current_arena_index];
}

Arena* ui_prev_build_arena()
{
  U32 prev_arena_index = (g_ui_state->current_arena_index == 0 ? 1 : 0);
  return g_ui_state->ui_tree_build_arenas[prev_arena_index];
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

// ---

void ui_state_init(Win32_window* window)
{
  Arena* arena = arena_alloc(Kilobytes_U64(20), "UI state arena");
  g_ui_state = ArenaPush(arena, UI_state);
  g_ui_state->perm_state_arena = arena;
  g_ui_state->window = window;

  g_ui_state->current_arena_index = 0;
  g_ui_state->ui_tree_build_arenas[0] = arena_alloc(Kilobytes_U64(64), "UI tree build arena 1");
  g_ui_state->ui_tree_build_arenas[1] = arena_alloc(Kilobytes_U64(64), "UI tree build arena 2");
}

void ui_state_release()
{
  arena_release(g_ui_state->perm_state_arena);
  arena_release(g_ui_state->ui_tree_build_arenas[0]);
  arena_release(g_ui_state->ui_tree_build_arenas[1]);
  g_ui_state = 0;
}

// ----

UI_Box* ui_get_box_with_key_opt(UI_Box* root, Str8 key)
{
  UI_Box* box = 0;

  if (str8_match(key, root->key, Str8_match_flag_NONE))
  {
    box = root;
  }
  else {
    for (UI_Box* child = root->first; child != 0; child = child->next)
    {
      box = ui_get_box_with_key_opt(child, key);
      if (box) {
        break;
      }
    }
  }

  return box;
}

B32 test_inputs_for_box(UI_Box* box)
{
  B32 result = false;
  Event_list* os_event_list = os_frame_event_list_from_window(g_ui_state->window);
  for (Event_node* node = os_event_list->first; node != 0; node = node->next)
  {
    Event* event = &node->event;

    if (   event->mouse_key_pressed 
        && rect_does_intersect_with_point(box->computed_final_rect, vec2_f32(event->mouse_x, event->mouse_y))
    ) {
      result = true;
      DllPopNode(os_event_list, node);
      break;
    }
  }
  return result;
}

B32 ui_is_clicked()
{
  // There might not have been a tree to present the user with yet
  if (g_ui_state->prev_frame_root == 0) 
  {
    return false;
  }

  UI_Box* prev_frame_ui_tree_root = g_ui_state->prev_frame_root;
  UI_Box* current_box = g_ui_state->current_parent;
  UI_Box* found_box = ui_get_box_with_key_opt(prev_frame_ui_tree_root, current_box->key);
  
  B32 is_clicked = false;
  if (found_box)
  {
    is_clicked = test_inputs_for_box(found_box);
  }
  return is_clicked;
}


// ---

UI_Box* ui_allocate_box_helper(
  Arena* arena, 
  UI_size size_kind_x, 
  UI_size size_kind_y, 
  Axis2 alignment_axis, 
  Color color,
  const char* key
) {
  UI_Box* box = ArenaPush(arena, UI_Box);;

  box->semantic_size[Axis2_x] = size_kind_x;
  box->semantic_size[Axis2_y] = size_kind_y;
  box->alignment_axis = alignment_axis;
  box->color = color;
  box->key = str8_from_cstr(arena, key);
  
  DllPushBack(g_ui_state->current_parent, box);
  g_ui_state->current_parent->children_count += 1;

  box->parent = g_ui_state->current_parent; 
  g_ui_state->current_parent = box;

  return box;
}

void ui_begin_build()
{
  // Get the new arena for the build
  // Clear it 
  // Allocated all the boxes on it
  g_ui_state->prev_frame_root = g_ui_state->root;
  g_ui_state->current_arena_index = (g_ui_state->current_arena_index == 0 ? 1 : 0);
  Arena* tree_arena = ui_current_build_arena(); 
  arena_clear(tree_arena);

  // TODO: I dont like this call here
  Rect ui_rect = *g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right;

  UI_Box* new_root = ArenaPush(tree_arena, UI_Box);
  new_root->semantic_size[Axis2_x] = UI_size{UI_size_kind_px, ui_rect.width};
  new_root->semantic_size[Axis2_y] = UI_size{UI_size_kind_px, ui_rect.height};
  new_root->alignment_axis = Axis2_x;
  new_root->color = C_BLACK;

  new_root->key = Str8FromClit(tree_arena, "ROOT_KEY_FOR_UI");

  g_ui_state->root = new_root;
  g_ui_state->current_parent = new_root;

  g_ui_state->padding_stack = ArenaPush(tree_arena, UI_padding_stack);
  ui_push_padding(25);

  g_ui_state->child_gap_stack = ArenaPush(tree_arena, UI_child_gap_stack);
  ui_push_child_gap(10);
}

void ui_end_build()
{
  // Nothing here for now, ui is only valid for a single build

}

// ---
 
UI_Box* ui_begin_box(
  UI_size size_kind_x, 
  UI_size size_kind_y, 
  Axis2 alignment_axis, 
  Color color,
  const char* key
) {
  UI_Box* new_box = ui_allocate_box_helper(ui_current_build_arena(), 
                                           size_kind_x, size_kind_y, 
                                           alignment_axis, color, key);
  return new_box;
}

void ui_end_box()
{
  g_ui_state->current_parent = g_ui_state->current_parent->parent;
}

// ---

void ui_sizing_pass(UI_Box* root, Axis2 axis)
{
  switch (root->semantic_size[axis].kind)
  {
    default: { InvalidCodePath(); } break;
    
    case UI_size_kind_px: 
    {
      root->computed_sizes[axis] = root->semantic_size[axis].value;
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_pass(child, axis); // This is depth first, we know the parent before the child
      }
    } break;

    case UI_size_kind_children_sum:
    {
      if (root->alignment_axis == axis)
      {
        // This is breadth first 
        F32 total_children_size = 0.0f;
        for (UI_Box* child = root->first; child != 0; child = child->next)
        {
          ui_sizing_pass(child, axis);
          total_children_size += child->computed_sizes[axis];
        } 
        if (root->children_count > 0)
        {
          total_children_size += ui_current_padding() * 2;
          total_children_size += ui_current_child_gap() * (root->children_count - 1);
        }
        root->computed_sizes[axis] = total_children_size; 
      }
      else 
      { 
        F32 max_child_size = 0.0f;
        for (UI_Box* child = root->first; child != 0; child = child->next)
        {
          ui_sizing_pass(child, axis);
          max_child_size = Max(max_child_size, child->computed_sizes[axis]);
        } 
        root->computed_sizes[axis] = max_child_size;
        if (root->children_count > 0)
        {
          root->computed_sizes[axis] += ui_current_padding() * 2;
        }
      }
    } break;

    case UI_size_kind_percent_of_parent:
    {
      F32 parent_size = root->parent->computed_sizes[axis];
      parent_size -= ui_current_padding() * 2;
      root->computed_sizes[axis] = parent_size * root->semantic_size[axis].value;
    } break;
    
    case UI_size_kind_text:
    {
      

      // Calculate the text width and height
      // Apply all the paddings and child gaps
      // Set the sizes for the sizing pass
    } break;  
  
  }

}

// ---

void ui_layout_pass(UI_Box* root, Axis2 axis)
{
  // if (root->size_kind[axis] == UI_size_px)
  // {
    F32 root_relative_pos = ui_current_padding();
    U32 child_index = 0;
    for (UI_Box* child = root->first; child != 0; child = child->next)
    {
      if (root->alignment_axis == axis)
      {
        child->computed_parent_rel_pos[axis] = root_relative_pos;
        root_relative_pos += ui_current_child_gap();
        root_relative_pos += child->computed_sizes[axis];
      }
      else
      {
        child->computed_parent_rel_pos[axis] = root_relative_pos;
      }

      ui_layout_pass(child, axis);
    }
  // }
}

// ---

void ui_final_pos_pass(UI_Box* root)
{
  local F32 x_offset = 0.0f;
  local F32 y_offset = 0.0f;

  root->computed_final_rect.x = x_offset + root->computed_parent_rel_pos[Axis2_x];
  root->computed_final_rect.y = y_offset + root->computed_parent_rel_pos[Axis2_y];
  root->computed_final_rect.width = root->computed_sizes[Axis2_x];
  root->computed_final_rect.height = root->computed_sizes[Axis2_y];

  x_offset += root->computed_parent_rel_pos[Axis2_x];
  y_offset += root->computed_parent_rel_pos[Axis2_y];

  for (UI_Box* child = root->first; child != 0; child = child->next)
  {
    ui_final_pos_pass(child);
  }

  x_offset -= root->computed_parent_rel_pos[Axis2_x];
  y_offset -= root->computed_parent_rel_pos[Axis2_y];
} 

// ---

void ui_draw_ui_helper(UI_Box* root)
{ 
  draw_rect(root->computed_final_rect, root->color);

  for (UI_Box* box = root->first; box !=0; box = box->next)
  {
    ui_draw_ui_helper(box);
  }
}

// ---


void ui_draw_ui()
{
  ui_sizing_pass(g_ui_state->root, Axis2_x);
  ui_sizing_pass(g_ui_state->root, Axis2_y);
  ui_layout_pass(g_ui_state->root, Axis2_x);
  ui_layout_pass(g_ui_state->root, Axis2_y);
  ui_final_pos_pass(g_ui_state->root);
  ui_draw_ui_helper(g_ui_state->root);
}






#endif