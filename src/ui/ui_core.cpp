#ifndef D_UI_CPP
#define D_UI_CPP

#include "base/include.cpp"
#include "os/core/os_core_win32.h"
#include "ui_core.h"

///////////////////////////////////////////////////////////
// Damian: Extra accesors
//
UI_size ui_size_make(UI_size_kind kind, F32 value)
{
  UI_size result = {};
  result.kind = kind;
  result.value = value;
  return result;
}

UI_size ui_size_px_make(F32 value)
{
  UI_size result = ui_size_make(UI_size_kind_px, value);
  return result;
}

UI_size ui_size_child_sum_make()
{
  UI_size result = ui_size_make(UI_size_kind_children_sum, Null);
  return result;
}

UI_size ui_size_text_make()
{
  UI_size result = ui_size_make(UI_size_kind_text, Null);
  return result;
}

UI_size ui_size_percent_of_parent_make(F32 p)
{
  UI_size result = ui_size_make(UI_size_kind_percent_of_parent, p);
  return result;
}

UI_size ui_size_fit_the_parent_make()
{
  UI_size result = ui_size_make(UI_size_kind_fit_the_parent, Null);
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
  
  box->key = str8_from_str8(arena, key);
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
    box->text = str8_from_str8(arena, text); 
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
  UI_Box* box = ui_allocated_and_set_up_box(str8_from_cstr(scratch.arena, key), flags, text);
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
/* NOTES: (November 12th 2025)
  -- Sizing pass to create sizes for things
   -- Hardcoded elements (px size, text size)    (padding + gaps)
   -- Child dependent elements (sum_of_children) (padding + gaps) 
   -- Parent dependent (%_of_parent, fit_parent) (padding + gaps)
    -- Elements with dinamic sizes, will have to be recalculated again (padding + gaps)
       |- For this we can just do the same calculation as before, 
       |- its just that now we have the parent dependant element sizes be non 0
  -- Layout pass to create offsets from parents
  -- Final pass to create the on screen rects
*/

// TODO
// TEST
// F32 ui_get_size_accounted_for_padding_and_gap(UI_Box* root, Axis2 axis, F32 total_children_sum)
// {
//   if (root->children_count > 0)
//   {
//     total_size_on_axis += 2 * root->padding; //ui_current_padding();
//     if (root->layout_axis == axis)
//     {
//       total_size_on_axis += (root->children_count - 1) * root->child_gap; //ui_current_child_gap();
//     }
//   }
// }

// F32 ui_DEBUG_get_total_children_size(UI_Box* box, Axis2 axis)
// {
//   F32 total_children_sum = 0.0f;
//   if (box->layout_axis == axis)
//   {
//     for (UI_Box* child = box->first; child != 0; child = child->next)
//     {
//       total_children_sum += child->computed_sizes[axis];
//     }
//   }
//   else 
//   {
//     total_children_sum = 
//   }
//   return total_children_sum;
// } 

// F32 normal_parent_total_padding_gap_size = ui_DEBUG_get_total_padding_gap_size(normal_parent, axis);

// F32 ui_DEBUG_get_total_space_left(UI_Box* root, Axis2 axis)
// {
//   // Size has to be computes for us to know how much space is left or overused
//   Assert(root->is_size_computed); 

//   F32 total_space_used = 0.0f;
//   if (root->layout_axis == axis)
//   {
//     for (UI_Box* child = root->first; child != 0; child = child->next)
//     {
//       total_space_used += child->computed_sizes[axis];
//     }
//     if (root->children_count > 0) 
//     {
//       total_space_used += 2 * root->padding;
//       if (root->children_count > 1) 
//     }
//   }
//   else 
//   {
//     total_children_sum = 
//   }
//   return total_children_sum;
// }

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


      // Damian: Padding and stuff.
      //         This is here manually since child_sum sizing requires special bahaviour that cant be pulled out nicely.
      //         In particular, it doesnt use padding if there are no children 
      // TODO: See that this does if i have no children inside such element and then on the sides normal elements, child gap will be used twice
      ///      Maybe it would be nice in this case to just remove this box from the tree, since it is zero sized in both axes
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
      // NotImplemented(); // Dont want to deal with this along fit parent at the same time for now

      UI_Box* usable_parent = 0;
      for (UI_Box* test_parent = root->parent; test_parent != 0; test_parent = test_parent->parent)
      {
        if (test_parent->semantic_size[axis].kind != UI_size_kind_children_sum) 
        {
          usable_parent = test_parent;
          break;
        }
      }

      // TODO: Paddng and child gap for root 1 
      F32 ui_DEBUG_get_box_usable_spece_left();

      // TODO: I dont know yet, weather i want to be getting the * value here for the usable parent or the root tho
      F32 size = usable_parent->computed_sizes[axis] * root->semantic_size[axis].value;
      size -= 2 * usable_parent->padding;
      if (usable_parent->layout_axis == axis && usable_parent->children_count > 0)
      {
        size -= (usable_parent->children_count - 1) * root->child_gap; 
      } 
      root->computed_sizes[axis] = size;

      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_parent_dependant_elements(child, axis);
      }

    } break;

    case UI_size_kind_fit_the_parent:
    {
      U32 children_count_with_fit_the_parent_sizing_scheme = 0;
      for (UI_Box* child = root->parent->first; child != 0; child = child->next)
      {
        if (child->semantic_size[axis].kind == UI_size_kind_fit_the_parent) {
          children_count_with_fit_the_parent_sizing_scheme += 1;
        }
      }
      if (children_count_with_fit_the_parent_sizing_scheme > 1)
      {
        // Damian: Multiple spacers inside a single child parent is not yet supported
        NotImplemented();
      }

      // Code path for stretching to an immidiate parent with px like width
      if (root->parent->semantic_size[axis].kind != UI_size_kind_children_sum)
      {
        // Parent pusts in its layout axis either way every time, where do i stretch then
        // If its alyout axis is x and i stretch in x, then i have to get the children sum, get the parent size, get the size left and stretch to that space
        // If i stretch in y then i just use the space that was given to me in x and in y i just become the size of the parent in y
        // X here is the parent layout axis and y is not, so just do == and != for that

        // TODO: See how elemetns calculate fit eleements, do they aply padding and child gap to them.
        //       What if there is no space to fit, then child gap still gets applied tho.

        // TODO: What if i have elements inside fit the parent, then what?
        //       How are they all sizes and it what order then?

       // TODO: Paddng and child gap for root 1 
        UI_Box* normal_parent = root->parent;
        F32 total_size = normal_parent->computed_sizes[axis];
        F32 size_used = 0.0f;

        if (normal_parent->layout_axis == axis) // Here we stretch based on the space left
        { 
          for (UI_Box* child = normal_parent->first; child != 0; child = child->next)
          {
            size_used += child->computed_sizes[axis];
          }
          size_used += 2 * normal_parent->padding;
          if (normal_parent->children_count > 0)
          {
            size_used -= (normal_parent->children_count - 1) * normal_parent->child_gap;
          }
        }
        else // Here we just get the other size for the parent, since that no where he lays out elements
        { 
          size_used -= 2 * normal_parent->padding;
        }
        root->computed_sizes[axis] = total_size - size_used;
      }
      else 
      { // Code path for stretching to a non immidiate parent

        /* NOTES for the case when we have fit inside child sum, but child sum is immediately inside the px like sizes box
        We want to get the usable px sized parent to get the size inside of it that is not used by elements.
        A part of the takes size for that usable parent will be takes by out child sum sized immediate parent.
        The size left for the usable parent we get is the size that we have to extend its (our) child sum box to.
        Now we know the final size of the child sum box, but we still need the size for the fit box, so the elements inside the child sum box are positioned correctly.
        We do the same here, we get all the children sums, paddings, gaps and fits the space we have left for children elements. 
        Then we just set value to be the size on that axis for the fit parent sized box.

        // TODO: Dont yet know how it shoud work when we have nested fitting
        */

        UI_Box* usable_parent = 0;
        for (UI_Box* test_parent = root->parent; test_parent != 0; test_parent = test_parent->parent)
        {
          if (test_parent->semantic_size[axis].kind != UI_size_kind_children_sum)
          {
            usable_parent = test_parent;
            break;
          }
        }
        Assert(usable_parent->semantic_size[axis].kind != UI_size_kind_fit_the_parent); // Damian: Not sure yet what this means semantically
        Assert(usable_parent);

        if (usable_parent == root->parent->parent)
        { 
          // Get the size left in the parent to fill up
          F32 usable_parent_total_size = usable_parent->computed_sizes[axis];
          F32 usable_parent_used_space = 0.0f;
          {
            // If we stretch to the layou axis of the parent then we need to accound for children sizes
            // Otherwise we just stretch to the size of the non layout axis whitch is sigle floored kind of in a way
            if (usable_parent->layout_axis == axis)
            {
        // TODO: Padding and child gap for root 1 

              for (UI_Box* child = usable_parent->first; child != 0; child = child->next)
              {
                usable_parent_used_space += child->computed_sizes[axis];
              }
              usable_parent_used_space += 2 * usable_parent->padding;
              if (usable_parent->children_count > 1)
              {
                usable_parent_used_space += (usable_parent->children_count - 1) * usable_parent->child_gap;
              }
            }
            else
            {
              usable_parent_used_space += 2 * usable_parent->padding;
            }
          }

          // Getting the size left in the root->parent children to see what we can stretch into inside of it
          F32 children_sum_parent_total_size = usable_parent_total_size - usable_parent_used_space; // TODO: What if its already bigger than this and overfowing
          F32 children_sum_parent_used_size = 0.0f;
          {
            // We here know that the the axis into which we stretch is the same axis for out child sum parent for children sum axis,
            // But out children sum parent still has a layout axis, so we have to accound for that.
            if (root->parent->layout_axis == axis)
            {
            // TODO: Paddng and child gap for root 1 
              
              for (UI_Box* child = root->parent->first; child != 0; child = child->next)
              {
                children_sum_parent_used_size += child->computed_sizes[axis];
              }
              children_sum_parent_used_size += 2 * root->parent->padding;
              if (root->parent->children_count > 1)
              {
                children_sum_parent_used_size += (root->parent->children_count - 1) * root->parent->child_gap;
              }
            }
            else 
            {
              children_sum_parent_used_size += 2 * root->parent->padding;
            }
          }

          // Getting the size left for us to stretch into
          F32 size_left_to_stretch_into = children_sum_parent_total_size - children_sum_parent_used_size; // TODO: What if its already bigger than this and overfowing
          root->computed_sizes[axis] = size_left_to_stretch_into;
        }
        else // Multi layer fitting is not yet supported
        {
          NotImplemented();
        }


      }

      // Get the usable parent to fit to (TODO: This might need a better comment here)
      UI_Box* usable_parent = 0;
      for (UI_Box* test_parent = root->parent; test_parent != 0; test_parent = test_parent->parent)
      {
        if (test_parent->semantic_size[axis].kind != UI_size_kind_children_sum) 
        {
          usable_parent = test_parent;
          break;
        }
      }

      for (UI_Box* child = root->first; child != 0; child = child->next)
      {
        ui_sizing_for_parent_dependant_elements(child, axis);
      }
      
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
  if (str8_match(root->key, Str8FromClit(scratch.arena, "Clay like box"), Str8_match_flag_NONE))
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
    Assert(root->computed_final_rect.width == dims.x);
    Assert(root->computed_final_rect.height == dims.y);

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
// Damian: Some manuall settings for boxes
//         Dont yet know what i want to use more, so implementing both, exploring
//
// void ui_set_text            (Str8 text)              { g_ui_state->current_parent->text            = text;             }
// void ui_set_text_color      (Color text_color)       { g_ui_state->current_parent->text_color      = text_color;       }
// void ui_set_flags           (UI_box_flags flags)     { g_ui_state->current_parent->flags           = flags;            }
// void ui_set_background_color(Color background_color) { g_ui_state->current_parent->backgound_color = background_color; }
// void ui_set_padding_color   (Color padding_color)    { g_ui_state->current_parent->padding_color   = padding_color;    }
// void ui_set_child_gap_color (Color child_gap_color)  { g_ui_state->current_parent->child_gap_color = child_gap_color;  }
// void ui_set_padding         (F32 padding)            { g_ui_state->current_parent->padding         = padding;          }
// void ui_set_child_gap       (F32 child_gap)          { g_ui_state->current_parent->child_gap       = child_gap;        }

// void ui_draw_padding() { g_ui_state->current_parent->flags |= UI_box_flag__draw_padding; }







#endif