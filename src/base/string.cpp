#ifndef BASE_STRING_CPP
#define BASE_STRING_CPP

#include "string.h"
#include "arena.cpp"
#include "math.cpp"

///////////////////////////////////////////////////////////
// Damian: Data_buffer stuff
//
Data_buffer data_buffer_make(Arena* arena, U64 size)
{
  Data_buffer buffer = {};
  buffer.count = size;
  buffer.data = ArenaPushArr(arena, U8, size);
  return buffer;
}

///////////////////////////////////////////////////////////
// Damian: Extra cstr helpers
//
U64 cstr_len(const char* name)
{
  U64 len = 0;
  while (name[len] != '\0') {
    len += 1;
  }
  return len;
}

///////////////////////////////////////////////////////////
// Damian: String stuff
//
// Damian: This is here insted of regular {}, since i usually use {} to represent the non yet given value. 
//         If i want in code to specify that i specifically want the value to be, 
//         and it to be zero, then i want it to be reflected in the code.
Str8 str8_empty() 
{
  Str8 str = {};
  return str;
}

Str8 str8_from_cstr_len(Arena* arena, const char* cstr, U64 len)
{
  Str8 str = {};
  str.data  = ArenaPushArr(arena, U8, len);
  str.count = len;
  MemCopy(str.data, cstr, str.count);
  U8* nt = ArenaPush(arena, U8);
  *nt = '\0';
  arena_pop(arena, 1);
  return str;
}

Str8 str8_from_cstr(Arena* arena, const char* cstr)
{
  Str8 str = str8_from_cstr_len(arena, cstr, cstr_len(cstr));
  return str;
} 

Str8 str8_from_str8(Arena* arena, Str8 str8)
{
  Str8 result = str8_from_cstr_len(arena, (char*)str8.data, str8.count);
  return result; 
}

Str8 str8_temp_from_cstr(const char* cstr)
{
  Scratch scratch = get_scratch();
  Str8 str = str8_from_cstr(scratch.arena, cstr);
  end_scratch(&scratch);
  return str;
}

Str8 str8_temp_from_str8(Str8 other)
{
  Scratch scratch = get_scratch();
  Str8 str = str8_from_str8(scratch.arena, other);
  end_scratch(&scratch);
  return str;
}

U8 char_to_lower(U8 ch)
{
  U8 result = ch;
  if ('A' <= ch && ch <= 'Z') {
    result = ch - ('A' - 'a');
  }
  return result;
}

U8 char_to_upper(U8 ch)
{
  U8 result = ch;
  if ('a' <= ch && ch <= 'z') {
    result = ch + ('A' - 'a');
  }
  return result;
}

U8 normalise_slash(U8 ch)
{
  U8 result = ch;
  if (ch == '\\') {
    result = '/';
  }
  return result;
}

void str8_list_push_str(Arena* arena, Str8_list* list, Str8 str)
{
  Str8_node* node = ArenaPush(arena, Str8_node);
  node->str = str;
  DllPushBack(list, node);
  list->node_count += 1;
  list->char_count += str.count;
}

Str8 str8_from_list(Arena* arena, Str8_list* list)
{
  Str8 str = data_buffer_make(arena, list->char_count);
  U64 current_index = 0;
  for (Str8_node* node = list->first; node != 0; node = node->next)
  {
    MemCopy(str.data + current_index, node->str.data, node->str.count);
    current_index += node->str.count;
  }
  // Damian: I would rather not, but i dont have a way to not manually null terminate here
  U8* nt = ArenaPush(arena, U8);
  *nt = '\0';
  arena_pop(arena, 1);
  return str;
}

B32 str8_match(Str8 str, Str8 other, U32 flags)
{
  B32 result = true;
  if (str.count != other.count) 
  {
    result = false;
  }
  else 
  {
    ForEachEx(char_index, str.count, str.data) 
    {
      U8 str_char = str.data[char_index];
      U8 other_char = other.data[char_index];
      if (flags & Str8_match_flag_ignore_case) {
        str_char = char_to_lower(str_char);
        other_char = char_to_lower(other_char);
      }
      if (flags & Str8_match_flag_normalise_slash)
      {
        str_char = normalise_slash(str_char);
        other_char = normalise_slash(other_char);
      }
      if (str_char != other_char)
      {
        result = false;
        break;
      }
    }
  }
  return result;
}

B32 str8_match_cstr(Str8 str, const char* c_str, U32 flags)
{
  B32 result = false;
  Scratch scratch = get_scratch();
  {
    Str8 other_str = str8_from_cstr(scratch.arena, c_str);
    result = str8_match(str, other_str, flags);
  } 
  end_scratch(&scratch);
  return result;
}

Str8 str8_substring(Str8 str, U64 start_index, U64 end_index)
{
  Str8 result = {};
  {
    if (start_index > end_index) {
      SwapVaues(U64, start_index, end_index);
    }
    end_index = Min(end_index, str.count);
    if (start_index < end_index)
    {
      result.data = str.data + start_index;
      result.count = end_index - start_index;
    }
  }
  return result;
}

Str8_list str8_split_by_str8(Arena* arena, Str8 str, Str8 sep, U32 flags)
{
  Str8_list list = {};
  if (str.count > sep.count) 
  {
    B32 is_sub_string_left = false;
    U64 sub_string_start_index = 0;
    ForEachEx(char_index, str.count, str.data)
    {
      is_sub_string_left = true;
      if ((str.count - char_index) < sep.count) { 
        break; 
      }
      Str8 test_sep = str8_substring(str, char_index, char_index + sep.count);
      if (str8_match(test_sep, sep, flags))
      {
        Str8 sub_str = str8_substring(str, sub_string_start_index, char_index);
        str8_list_push_str(arena, &list, sub_str);
        sub_string_start_index = char_index + test_sep.count; // Removing the separator as well
        char_index += test_sep.count;
        is_sub_string_left = false;
      }
    }
    if (is_sub_string_left)
    {
        Str8 sub_string = str8_substring(str, sub_string_start_index, str.count);
        str8_list_push_str(arena, &list, sub_string);
    }
  }
  return list;
}


Str8_list str8_split_by_cstr(Arena* arena, Str8 str, const char* sep, U32 flags)
{
  Str8_list list = {};
  Scratch scratch = get_scratch();
  {
    list = str8_split_by_str8(arena, str, Str8FromClit(scratch.arena, sep), flags);
  }
  end_scratch(&scratch);
  return list;
}

// TODO: These have to be moved to somewhere else or just renamed to confirm to the naming scheme of str8 stuff
Str8 get_file_basename(Str8 path)
{
  Str8 result = {};
  Scratch scratch = get_scratch();
  {
    Str8_list split_list = str8_split_by_str8(scratch.arena, 
                                              path, 
                                              Str8FromClit(scratch.arena, "/"), 
                                              Str8_match_flag_normalise_slash);
    if (split_list.node_count > 0)
    {
      result = split_list.last->str;
    }
  }
  end_scratch(&scratch);
  return result;
}

Str8 get_file_name(Str8 path)
{
  Str8 result = {};
  Str8 basename = get_file_basename(path);
  Scratch scratch = get_scratch();
  {
    Str8_list split_list = str8_split_by_str8(scratch.arena, 
                                              basename, 
                                              Str8FromClit(scratch.arena, "."), 
                                              Str8_match_flag_NONE);
    if (split_list.node_count > 0) 
    {
      result = split_list.first->str;
    }
  }
  end_scratch(&scratch);
  return result;
}

Str8 get_file_extension(Str8 path)
{
Str8 result = {};
  Str8 basename = get_file_basename(path);
  Scratch scratch = get_scratch();
  {
    Str8_list split_list = str8_split_by_str8(scratch.arena, 
                                              basename, 
                                              Str8FromClit(scratch.arena, "."), 
                                              Str8_match_flag_NONE);
    if (split_list.node_count > 0) 
    {
      result = split_list.last->str;
    }
  }
  end_scratch(&scratch);
  return result;
}








#endif