#ifndef D_UI_CPP
#define D_UI_CPP

#include "base/include.cpp"
#include "os/core/os_core_win32.h"
#include "ui_core.h"

///////////////////////////////////////////////////////////
// Damian: Extra accesors
//
UI_size ui_size_make(UI_size_kind kind, F32 value, F32 strictness)
{
  UI_size result = {};
  result.kind = kind;
  result.value = value;
  result.strictness = strictness;
  return result;
}

UI_size ui_size_px_make(F32 value, F32 strictness)
{
  UI_size result = ui_size_make(UI_size_kind_px, value, strictness);
  return result;
}

UI_size ui_size_child_sum_make()
{
  UI_size result = ui_size_make(UI_size_kind_children_sum, Null, Null);
  return result;
}

UI_size ui_size_text_make(F32 strictness)
{
  UI_size result = ui_size_make(UI_size_kind_text, Null, strictness);
  return result;
}

UI_size ui_size_percent_of_parent_make(F32 p)
{
  UI_size result = ui_size_make(UI_size_kind_percent_of_parent, p, Null);
  return result;
}

UI_size ui_size_fit_the_parent_make(F32 grow_value)
{
  UI_size result = ui_size_make(UI_size_kind_fit_the_parent, grow_value, Null);
  return result;
}

Arena* ui_current_build_arena()
{
  Arena* arena = g_ui_state->ui_tree_build_arenas[g_ui_state->current_arena_index]; 
  return arena;
}

Arena* ui_prev_build_arena()
{
  U32 prev_arena_index = (g_ui_state->current_arena_index == 0 ? 1 : 0);
  Arena* arena = g_ui_state->ui_tree_build_arenas[prev_arena_index]; 
  return arena;
}

/*

*/
// Str8 ui_key_from_str8(Str8 str)
// {
  
// }

// Str8 ui_key_from_cstr(const char* cstr)
// {
  
// }

///////////////////////////////////////////////////////////
// Damian: State
//
void ui_state_init(Win32_window* window, Font_info* font_info)
{
  Arena* arena = arena_alloc(Kilobytes_U64(20), "UI state arena");
  g_ui_state = ArenaPush(arena, UI_state);
  g_ui_state->perm_state_arena = arena;
  g_ui_state->window = window;
  g_ui_state->font_info = font_info;

  g_ui_state->current_arena_index = 0;
  g_ui_state->ui_tree_build_arenas[0] = arena_alloc(Megabytes_U64(64), "UI tree build arena 1");
  g_ui_state->ui_tree_build_arenas[1] = arena_alloc(Megabytes_U64(64), "UI tree build arena 2");
}

void ui_state_release()
{
  arena_release(g_ui_state->ui_tree_build_arenas[0]);
  arena_release(g_ui_state->ui_tree_build_arenas[1]);
  arena_release(g_ui_state->perm_state_arena);
  g_ui_state = 0;
}

///////////////////////////////////////////////////////////
// Damian: Debug Input stuff
//
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

UI_Box* ui_get_box()
{
  return g_ui_state->current_parent;
}


B32 test_inputs_for_box(UI_Box* box)
{
  B32 result = false;
  Event_list* os_event_list = os_frame_event_list_from_window(g_ui_state->window);
  for (Event_node* node = os_event_list->first; node != 0; node = node->next)
  {
    Event* event = &node->event;

    if (   event->mouse_key_pressed 
        && rect_does_intersect_with_point(box->computed_final_rect, vec2_f32((F32)event->mouse_x, (F32)event->mouse_y))
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

///////////////////////////////////////////////////////////
// Damian: UI element creation stuff 
//
UI_Box* ui_allocated_and_set_up_box(Str8 key, UI_box_flags flags, Str8 text) 
{
  Arena* arena = ui_current_build_arena();
  UI_Box* box = ArenaPush(ui_current_build_arena(), UI_Box);
  
  box->key = str8_from_str8_alloc(arena, key);
  box->semantic_size[Axis2_x] = ui_current_size_x();
  box->semantic_size[Axis2_y] = ui_current_size_y();
  box->layout_axis = ui_current_layout_axis();
  
  box->padding   = ui_current_padding();
  box->child_gap = ui_current_child_gap();
  
  box->flags = flags;
  if (flags & UI_box_flag__has_backgound)
  {
    box->backgound_color = ui_current_backgound_color();
  }
  if (flags & UI_box_flag__has_text)
  {
    box->text = str8_from_str8_alloc(arena, text); 
    box->text_color = ui_current_text_color(); 
  }
  if (flags & UI_box_flag__draw_padding)
  {
    box->padding_color = ui_current_padding_color(); 
  }
  if (flags & UI_box_flag__draw_child_gap)
  {
    box->child_gap_color = ui_current_child_gap_color(); 
  }


  // TODO: I dont like the different nature for stuff like draw_padding whitch we supposed to not store as a bool bug get from flags

  return box;
}

UI_Box* ui_allocated_and_set_up_box(const char* key, UI_box_flags flags, Str8 text)
{
  Scratch scratch = get_scratch();  
  UI_Box* box = ui_allocated_and_set_up_box(str8_from_cstr(key), flags, text);
  end_scratch(&scratch);
  return box;
}

void ui_begin_build()
{
  g_ui_state->prev_frame_root = g_ui_state->root;
  g_ui_state->current_arena_index = (g_ui_state->current_arena_index == 0 ? 1 : 0);
  Arena* tree_arena = ui_current_build_arena(); 
  arena_clear(tree_arena);

  // TODO: I dont like this call here
  Rect ui_rect = win32_get_client_area_rect(g_ui_state->window);

  #define UI_STACK_DATA(stack_struct_name, stack_var_name,                    \
                        node_struct_name,  node_var_name,                     \
                        Value_type, value_var_name, default_value,            \
                        push_func_name, pop_func_name, get_current_func_name) \
  g_ui_state->stack_var_name = ArenaPush(tree_arena, stack_struct_name); \
  push_func_name(default_value);                                         
  UI_STACK_DATA_TABLE
  #undef UI_STACK_DATA

  UI_box_flags root_flags = UI_box_flag__has_backgound;
  UI_Box* new_root = ui_allocated_and_set_up_box("ROOT_KEY_FOR_UI", root_flags, str8_empty());
  g_ui_state->root = new_root;
  g_ui_state->current_parent = new_root;

  // TODO: This hardcoded here has to go
  new_root->semantic_size[Axis2_x].value = ui_rect.width;
  new_root->semantic_size[Axis2_y].value = ui_rect.height;
}

void ui_end_build()
{
  // Nothing here for now, ui is only valid for a single build
}

#define UI_Key(value) #value

UI_Box* ui_begin_box(Str8 key, UI_box_flags flags, Str8 text) 
{
  UI_Box* new_box = ui_allocated_and_set_up_box(key, flags, text);
  DllPushBack(g_ui_state->current_parent, new_box);
  g_ui_state->current_parent->children_count += 1;

  new_box->parent = g_ui_state->current_parent; 
  g_ui_state->current_parent = new_box;

  return new_box;
}

void ui_end_box()
{
  g_ui_state->current_parent = g_ui_state->current_parent->parent;
}

///////////////////////////////////////////////////////////
// Damian: UI sizing/layout/pos stuff
//
void ui_sizing_for_fixed_sized_elements(UI_Box* root, Axis2 axis)
{
  switch (root->semantic_size[axis].kind)
  {
    default: {
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_fixed_sized_elements(child, axis);
      }
    } break;

    case UI_size_kind_px:
    {
      root->computed_sizes[axis] = root->semantic_size[axis].value;
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_fixed_sized_elements(child, axis);
      }
    } break;

    case UI_size_kind_text:
    {
      Vec2_F32 dims = font_measure_text(g_ui_state->font_info, root->text);
      root->computed_sizes[axis] = dims.values[axis];
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_fixed_sized_elements(child, axis);
      }
    } break;
  }

}

void ui_sizing_for_child_dependant_elements(UI_Box* root, Axis2 axis)
{
  switch (root->semantic_size[axis].kind)
  {
    default: {
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_child_dependant_elements(child, axis);
      }
    } break;

    case UI_size_kind_children_sum:
    {
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_child_dependant_elements(child, axis);
      }

      F32 total_size_on_axis = 0.0f;
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        if (root->layout_axis == axis) {
          total_size_on_axis += child->computed_sizes[axis];
        } else {
          total_size_on_axis = Max(total_size_on_axis, child->computed_sizes[axis]);
        }
      } 

      if (root->children_count > 0)
      {
        total_size_on_axis += 2 * root->padding; 
        if (root->layout_axis == axis)
        {
          total_size_on_axis += (root->children_count - 1) * root->child_gap; 
        }
      }
    
      root->computed_sizes[axis] = total_size_on_axis;

    } break;
  }

}

// ------------

F32 ui_formal_total_space_from_box(UI_Box* box, Axis2 axis)
{
  F32 space = box->computed_sizes[axis];
  return space;
}

F32 ui_real_total_space_from_box(UI_Box* box, Axis2 axis)
{
  F32 space = 0.0f;

  // Getting space taken by children for that axis 
  if (box->layout_axis == axis)
  {
    for (UI_Box* child = box->first; child != 0; child = child->next)
    {
      space += child->computed_sizes[axis];
    }
  }
  else
  {
    for (UI_Box* child = box->first; child != 0; child = child->next)
    {
      space = Max(space, child->computed_sizes[axis]); 
    }
  }
  
  // Accounting for padding and child gap
  if (box->semantic_size[axis].kind == UI_size_kind_children_sum)
  {
    if (box->children_count > 0)
    {
      space += 2 * box->padding;
    }
    if (box->layout_axis == axis && box->children_count > 1)
    {
      space += (box->children_count - 1) * box->child_gap;
    }
  }
  else 
  {
    space += 2 * box->padding;
    if (box->layout_axis == axis && box->children_count > 1)
    {
      space += (box->children_count - 1) * box->child_gap;
    }
  }
 
  return space;
}

// ------------

F32 ui_paddin_and_gaps_space_from_box(UI_Box* box, Axis2 axis)
{
  F32 space = 0.0f;
  if (box->semantic_size[axis].kind == UI_size_kind_children_sum)
  {
    if (box->children_count > 0) {
      space += 2 * box->padding;
    }
  }
  else
  {
    space += 2 * box->padding;
  }
  if (box->layout_axis == axis && box->children_count > 1) {
    space += (box->children_count - 1) * box->padding;
  }
  return space;
}

// ------------

F32 ui_formal_child_space_from_box(UI_Box* box, Axis2 axis)
{
  F32 formal_total_space = ui_formal_total_space_from_box(box, axis);
  F32 padding_and_gaps = ui_paddin_and_gaps_space_from_box(box, axis); 
  F32 formal_child_space = formal_total_space - padding_and_gaps; 
  return formal_child_space;
}

F32 ui_real_child_space_from_box(UI_Box* box, Axis2 axis)
{
  F32 real_total_space = ui_real_total_space_from_box(box, axis);
  F32 padding_and_gaps = ui_paddin_and_gaps_space_from_box(box, axis); 
  F32 real_child_space = real_total_space - padding_and_gaps; 
  return real_child_space;
}

// ------------

void ui_sizing_for_parent_dependant_elements(UI_Box* root, Axis2 axis)
{
  switch (root->semantic_size[axis].kind)
  {
    default: {
      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_parent_dependant_elements(child, axis);
      }
    } break;

    case UI_size_kind_percent_of_parent:
    {
      // TODO: Do we accont for padding and child gaps here tho?
      UI_Box* parent = root->parent;
      F32 parent_total_size = root->parent->computed_sizes[axis];
      F32 parent_usable_size = parent_total_size;
      root->computed_sizes[axis] = parent_total_size * root->semantic_size[axis].value; 

      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_parent_dependant_elements(child, axis);
      }

    } break;

    case UI_size_kind_fit_the_parent:
    {
      // Get the size left in the parent to fit into
      // Get the number of the children that might be stretched
      // Each 1 of then has to get stretched to the 100%/n of the parent space that is left to be filled in

      F32 total_possible_space_for_children = ui_formal_child_space_from_box(root->parent, axis);
      F32 total_used_space_for_children = ui_real_child_space_from_box(root->parent, axis);
      F32 space_left = total_possible_space_for_children - total_used_space_for_children;
      if (space_left > 0.0f)
      {
        U32 n_growers = 0;
        F32 total_growing_k = 0.0f;
        for (UI_Box* child = root->parent->first; child != 0; child = child->next)
        {
          if (child->semantic_size[axis].kind == UI_size_kind_fit_the_parent) {
            n_growers += 1;
            total_growing_k += child->semantic_size[axis].value;
          }
        }

        if (n_growers > 0)
        {
          F32 space_to_grow = space_left;
          if (root->parent->layout_axis == axis)
          {
            F32 root_grow_ration = root->semantic_size[axis].value / total_growing_k;
            space_to_grow = space_left * root_grow_ration;
          }

          root->computed_sizes[axis] += space_to_grow;
        }
      }

      // UI_Box* parent = root->parent;
      // F32 total_size = parent->computed_sizes[axis];
      // F32 size_used = 0.0f;

      // if (parent->layout_axis == axis) // Here we stretch based on the space left
      // { 
      //   for (UI_Box* child = parent->first; child != 0; child = child->next)
      //   {
      //     size_used += child->computed_sizes[axis];
      //   }
      //   size_used += 2 * parent->padding;
      //   if (parent->children_count > 0)
      //   {
      //     size_used += (parent->children_count - 1) * parent->child_gap;
      //   }
      // }
      // else // Here we just get the other size for the parent, since that no where he lays out elements
      // { 
      //   size_used += 2 * parent->padding;
      // }
      // root->computed_sizes[axis] = total_size - size_used;
      // root->computed_sizes[axis] = Max(0.0f, root->computed_sizes[axis]); // If its overflowing, then we just set the fit parent to 0, since it cant fit anymore
  
      // for (UI_Box* child = root->first; child != 0; child = child->next)
      // {
      //   ui_sizing_for_parent_dependant_elements(child, axis);
      // }
    } break;
  }

}

// ---

void ui_layout_pass(UI_Box* root, Axis2 axis)
{
  // TODO: This will get fucked if i decide to change the padding in the later calls, 
  //       all the ui will get affected by this, since it only uses the top padding
  F32 total_children_width_before = 0;
  U32 child_index = 0;
  for (UI_Box* child = root->first; child != 0; child = child->next)
  {
    if (root->layout_axis == axis)
    {
      child->computed_parent_rel_pos[axis] = root->padding /*ui_current_padding()*/ + total_children_width_before;
      child->computed_parent_rel_pos[axis] += child_index * root->child_gap; //ui_current_child_gap();
      total_children_width_before += child->computed_sizes[axis];
      child_index += 1;
    }
    else
    {
      // There can only be a single child in the axis that is not set for child aligning
      child->computed_parent_rel_pos[axis] = root->padding; //ui_current_padding();
    }

    ui_layout_pass(child, axis);
  }
}

// ---

void ui_final_pos_pass(UI_Box* root)
{
  local_persist F32 x_offset = 0.0f;
  local_persist F32 y_offset = 0.0f;

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

///////////////////////////////////////////////////////////
// Damian: Draw
//
void ui_draw_ui_helper(UI_Box* root)
{ 
  Scratch scratch = get_scratch();
  if (str8_match(root->key, Str8FromClit("Clay like box"), Str8_match_flag_NONE))
  {
    // DebugStopHere();
  }
  end_scratch(&scratch);

  if (root->flags & UI_box_flag__has_backgound)
  {
    draw_rect(root->computed_final_rect, root->backgound_color);
  }

  if (root->flags & UI_box_flag__has_text)
  {
    Vec2_F32 dims = font_measure_text(g_ui_state->font_info, root->text);
    
    // Damian: Removed these cause since now i have overflow, these dont really work when text gets shrinked but the measure text is still the same
    // Assert(root->computed_final_rect.width == dims.x);
    // Assert(root->computed_final_rect.height == dims.y);

    test_draw_text(
      g_ui_state->font_info, 
      g_ui_state->font_texture, 
      root->text, 
      root->computed_final_rect.x,
      root->computed_final_rect.y,
      root->text_color);
    
    #if 0
    test_draw_text_lines(
      g_ui_state->font_info, 
      g_ui_state->font_texture, 
      root->text, 
      root->computed_final_rect.x, 
      root->computed_final_rect.y);
    #endif
  }

  if (root->flags & UI_box_flag__has_texture)
  {
    // Load the image onto the gpu
    // Get the texture and draw the texture filling the area of the box
    Texture2D texture = root->texture;
    Rect texture_source_rect = rect_make(0.0f, 0.0f, (F32)texture.width, (F32)texture.height);
    Rect texture_dest_rect = root->computed_final_rect;
    test_draw_texture_pro(texture, texture_source_rect, texture_dest_rect);
  }

  if (root->flags & UI_box_flag__draw_padding)
  {
    F32 padding = root->padding;
    Rect r1 = rect_make(root->computed_final_rect.x, root->computed_final_rect.y, padding, root->computed_final_rect.height);
    Rect r2 = rect_make(root->computed_final_rect.x, root->computed_final_rect.y, root->computed_final_rect.width, padding);
    Rect r3 = rect_make(root->computed_final_rect.x + root->computed_final_rect.width - padding, root->computed_final_rect.y, padding, root->computed_final_rect.height);
    Rect r4 = rect_make(root->computed_final_rect.x, root->computed_final_rect.y + root->computed_final_rect.height - padding, root->computed_final_rect.width, padding);
    draw_rect(r1, root->padding_color);
    draw_rect(r2, root->padding_color);
    draw_rect(r3, root->padding_color);
    draw_rect(r4, root->padding_color);
  }

  // TODO: Make this codepath here better
  if (root->flags & UI_box_flag__draw_child_gap)
  {
    U32 child_index = 0;
    F32 children_total_size_on_align_axis_yet = 0.0f;
    for (UI_Box* child = root->first; child != 0; child = child->next)
    {
      Vec2_F32 root_pos = rect_pos(root->computed_final_rect);
      Vec2_F32 root_dims = rect_dims(root->computed_final_rect);
      Vec2_F32 child_dims = rect_dims(child->computed_final_rect);

      if (child_index > 0)
      {
        F32 padding = root->padding;
        F32 child_gap = root->child_gap;
        Rect r = {};
        if (root->layout_axis == Axis2_x)
        {
          r.x      = root_pos.values[Axis2_x] + padding + children_total_size_on_align_axis_yet + ((child_index - 1) * child_gap);
          r.y      = root_pos.values[Axis2_y] + padding;
          r.width  = ui_current_child_gap();
          r.height = root_dims.values[Axis2_y] - (2 * padding);
        }
        else if (root->layout_axis == Axis2_y)
        {
          r.x      = root_pos.values[Axis2_x] + padding;
          r.y      = root_pos.values[Axis2_y] + padding + children_total_size_on_align_axis_yet + ((child_index - 1) * child_gap);
          r.width  = root_dims.values[Axis2_x] - (2 * padding);
          r.height = child_gap;
        }
        else { InvalidCodePath(); }
        draw_rect(r, root->child_gap_color);
      }

      child_index += 1;
      children_total_size_on_align_axis_yet += child_dims.values[root->layout_axis];
    }
  }
  
  for (UI_Box* box = root->first; box !=0; box = box->next)
  {
    ui_draw_ui_helper(box);
  }
}



// TODO: Move this to a better place
void ui_fix_overflow_pass(UI_Box* root, Axis2 axis)
{
  F32 formal_child_space = ui_formal_child_space_from_box(root, axis);
  F32 real_child_space = ui_real_child_space_from_box(root, axis);
  F32 overflow = real_child_space - formal_child_space;
  if (overflow > 0.0f)
  {
    for (UI_Box* child = root->first; child != 0; child = child->next)
    {
      F32 strictness = child->semantic_size[axis].strictness;
      F32 p_to_remove = 1.0f - strictness;
      child->computed_sizes[axis] -= overflow * p_to_remove; 
    }

    // printf("Encountered an overflow at key: %s \n", str8_temp_from_str8(root->key).data);
  }

  // Recurse
  for (UI_Box* child = root->first; child != 0; child = child->next)
  {
    ui_fix_overflow_pass(child, axis);
  }
}

void ui_draw_ui()
{
  ui_sizing_for_fixed_sized_elements(g_ui_state->root, Axis2_x);
  ui_sizing_for_fixed_sized_elements(g_ui_state->root, Axis2_y);

  ui_sizing_for_child_dependant_elements(g_ui_state->root, Axis2_x);
  ui_sizing_for_child_dependant_elements(g_ui_state->root, Axis2_y);

  ui_sizing_for_parent_dependant_elements(g_ui_state->root, Axis2_x);
  ui_sizing_for_parent_dependant_elements(g_ui_state->root, Axis2_y);

  // The second pass to acound for some possible child element expansions
  ui_sizing_for_child_dependant_elements(g_ui_state->root, Axis2_x);
  ui_sizing_for_child_dependant_elements(g_ui_state->root, Axis2_y);

  // THIS IS NEW
  ui_fix_overflow_pass(g_ui_state->root, Axis2_x);
  ui_fix_overflow_pass(g_ui_state->root, Axis2_y);

  ui_layout_pass(g_ui_state->root, Axis2_x);
  ui_layout_pass(g_ui_state->root, Axis2_y);
  
  ui_final_pos_pass(g_ui_state->root);
  
  ui_draw_ui_helper(g_ui_state->root);
}

// TODO: This is temp
void ui_equip_font_texture(Texture2D font_texture)
{
  g_ui_state->font_texture = font_texture;
}

///////////////////////////////////////////////////////////
// Damian: Inputs
//
UI_Box* ui_box_from_key_opt(UI_Box* root, Str8 key)
{
  UI_Box* result = 0;
  if (str8_match(root->key, key, Str8_match_flag_NONE))
  {
    result = root;
  }

  if (!result)
  {
    for (UI_Box* child = root->first; child != 0; child = child->next)
    {
      result = ui_box_from_key_opt(child, key);
      if (result)
      {
        break;
      }
    }
  }

  return result;
}

/* NOTES:
  Kind of inputs i need:
  - Final mouse pos
  -
*/

UI_Inputs ui_get_inputs()
{
  UI_Inputs inputs = {};

  UI_Box* current = g_ui_state->current_parent;
  UI_Box* prev_root = g_ui_state->prev_frame_root;
  
  if (prev_root)
  {
    UI_Box* prev_current = ui_box_from_key_opt(prev_root, current->key);
    UI_Inputs prev_inputs = prev_current->inputs; 
    inputs = prev_inputs;

    if (prev_current)
    {
      Rect rect = prev_current->computed_final_rect;

      // Reseting the clicked from the prev frame
      inputs.is_clicked = false;

      // Is hovered 
      {
        Vec2 mouse_pos = g_ui_state->window->last_frame_final_mouse_pos;
        if (rect_does_intersect_with_point(rect, mouse_pos)) {
          inputs.is_hovered = true;
          inputs.withing_widget_mouse_pos = vec2_f32(mouse_pos.x - rect.x, mouse_pos.y - rect.y);
        } else {
          inputs.is_hovered = false;
        }
      }

      // Is pressed
      {
        Event_list* list = g_ui_state->window->frame_event_list;
        for (Event_node* node = list->first; node != 0; node = node->next)
        {
          Event* event = &node->event;
          if (   event->type == Event_type_mouse 
              && event->mouse_key_pressed == Mouse_key_left
          ) {
            Vec2 mouse_pos = vec2_f32((F32)event->mouse_x, (F32)event->mouse_y);
            if (rect_does_intersect_with_point(rect, mouse_pos))
            {
              inputs.is_pressed_left = true;
              inputs.withing_widget_mouse_pos = vec2_f32(mouse_pos.x - rect.x, mouse_pos.y - rect.y);
              break;
            }   
          }
        }
      }

      // Did get unpressed
      if (inputs.is_pressed_left)
      {
        Event_list* list = g_ui_state->window->frame_event_list;
        for (Event_node* node = list->first; node != 0; node = node->next)
        {
          Event* event = &node->event;
          if (   event->type == Event_type_mouse 
              && event->mouse_key_released == Mouse_key_left
          ) {
            inputs.is_pressed_left = false;
            break;
          }
        }
      }

      // Did get clicked
      if (!inputs.is_pressed_left && prev_inputs.is_pressed_left)
      {
        inputs.is_clicked = true;
        inputs.withing_widget_mouse_pos = prev_inputs.withing_widget_mouse_pos;
      }
      
      current->inputs = inputs;
    }

  }

  // local U64 frame_counter = 0;
  // printf("Frame: %lld --> %s \n", frame_counter++, (inputs.is_pressed_left ? "Pressed" : "Up"));

  return inputs;
}

///////////////////////////////////////////////////////////
// Damian: THESE ARE JUST SOME EXTRA THINGS I AM TESTING AND DONT WANT TO PUT IN OTHER FUNCS FOR NOW 
//
void ui_draw_padding_for_current(Color padding_color)
{
  g_ui_state->current_parent->flags = (UI_box_flags)(g_ui_state->current_parent->flags | UI_box_flag__draw_padding);
  g_ui_state->current_parent->padding_color = padding_color;
}

void ui_draw_child_gap_color(Color gap_color)
{
  g_ui_state->current_parent->flags = (UI_box_flags)(g_ui_state->current_parent->flags | UI_box_flag__draw_child_gap);
  g_ui_state->current_parent->child_gap_color = gap_color;
}

// void ui_set_min_size(F32 size, Axis2 axis)
// {
//   UI_Box* box = g_ui_state->current_parent;
//   box->min_size[axis] = size;
//   box->has_min_size[axis] = true;
// }

// void ui_set_max_size(F32 size, Axis2 axis)
// {
//   UI_Box* box = g_ui_state->current_parent;
//   box->max_size[axis] = size;
//   box->has_max_size[axis] = true;
// }

UI_Inputs ui_box_make(Str8 key, UI_box_flags flags, Str8 text)
{
  // TODO: See maybe ui_get_inputs not using the hidden element but expecting a parameter is better
  UI_Inputs inputs = {};
  UI_Box* box = ui_begin_box(key, flags, text);
  {
    inputs = ui_get_inputs();
  }
  ui_end_box();
  return inputs;
}

#define UI_BoxLoop(key, flags, text) DefereLoop(ui_begin_box(key, flags, text), ui_end_box())

///////////////////////////////////////////////////////////
// Damian: Stacks
//
// Defining stack push functions
#define UI_STACK_DATA(stack_struct_name, stack_var_name,                    \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
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
  #define UI_STACK_DATA(stack_struct_name, stack_var_name,                  \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
  void pop_func_name()                       \
  {                                          \
    StackPop(g_ui_state->stack_var_name);    \
    Assert(g_ui_state->stack_var_name->count >= 1); \
  }
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA

// Defining stack get_current functions
#define UI_STACK_DATA(stack_struct_name, stack_var_name,                    \
                      node_struct_name,  node_var_name,                     \
                      Value_type, value_var_name, default_value,            \
                      push_func_name, pop_func_name, get_current_func_name) \
  Value_type get_current_func_name()         \
  {                                          \
    Value_type value = g_ui_state->stack_var_name->first->value_var_name; \
    return value;                                                         \
  }
  UI_STACK_DATA_TABLE
#undef UI_STACK_DATA

///////////////////////////////////////////////////////////
// Damian: Setters 
//
void ui_set_background_color(Color color) { UI_Box* box = ui_get_current_element(); box->backgound_color = color;       }
void ui_set_child_gap(F32 value)          { UI_Box* box = ui_get_current_element(); box->child_gap = value;             }
void ui_set_child_gap_color(Color color)  { UI_Box* box = ui_get_current_element(); box->child_gap_color = color;       }
void ui_set_text_color(Color color)       { UI_Box* box = ui_get_current_element(); box->text_color = color;            }
void ui_set_padding(F32 value)            { UI_Box* box = ui_get_current_element(); box->padding = value;               }
void ui_set_padding_color(Color color)    { UI_Box* box = ui_get_current_element(); box->padding_color = color;         }
void ui_set_size_x(UI_size size)          { UI_Box* box = ui_get_current_element(); box->semantic_size[Axis2_x] = size; }
void ui_set_size_y(UI_size size)          { UI_Box* box = ui_get_current_element(); box->semantic_size[Axis2_y] = size; }
void ui_set_layout_axis(Axis2 axis)       { UI_Box* box = ui_get_current_element(); box->layout_axis = axis;            }

///////////////////////////////////////////////////////////
// Damian: Some private like things that i just migth want to use, will see
//
UI_Box* ui_get_current_element()
{
  return g_ui_state->current_parent;
}






#endif