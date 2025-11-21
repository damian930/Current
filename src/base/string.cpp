#ifndef BASE_STRING_CPP
#define BASE_STRING_CPP

#include "string.h"
#include "arena.cpp"
#include "math.cpp"

#include "string_fmt.h"
#include "string_fmt.cpp"

///////////////////////////////////////////////////////////
// Damian: Constructors 
//
Str8 str8_empty() 
{
  Str8 str = {};
  str.count = 0;
  str.data  = Null;
  return str;
}

Str8 str8_from_cstr_len(const char* cstr, U64 len)
{
  Str8 str = {};
  str.data = (U8*)cstr;
  str.count = len;
  return str;
}

Str8 str8_from_cstr(const char* cstr)
{
  U64 len = get_cstr_len(cstr);
  Str8 str = str8_from_cstr_len(cstr, len);
  return str;
}

Str8 str8_from_str8_alloc(Arena* arena, Str8 other_str8)
{
  // Create a buffer, copy the data, bull terminate, pop
  Str8 str = {};
  str.data = ArenaPushArr(arena, U8, other_str8.count);
  str.count = other_str8.count;
  MemCopy(str.data, other_str8.data, str.count);
  U8* nt = ArenaPush(arena, U8);
  *nt = '\0';
  arena_pop(arena, 1);
  return str;
}

Str8 str8_from_fmt_alloc(Arena* arena, const char* fmt, ...)
{
  va_list args = {};
  va_start(args, fmt);
  Str8 formated_str = str8_fmt_format(arena, fmt, args);
  va_end(args);
  return formated_str;
}


// ---------- TODO: This is to be marked when its done

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

B32 str8_match(Str8 str, Str8 other, Str8_match_flags flags)
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

B32 str8_match_cstr(Str8 str, const char* c_str, Str8_match_flags flags)
{
  B32 result = false;
  Scratch scratch = get_scratch();
  {
    Str8 other_str = str8_from_cstr(c_str);
    result = str8_match(str, other_str, flags);
  } 
  end_scratch(&scratch);
  return result;
}

Str8 str8_substring_range(Str8 str, Range_U64 range)
{
  Str8 result = {};
  {
    if (range.min > range.max) {
      SwapVaues(U64, range.min, range.max);
    }
    range.max = Min(range.max, str.count);
    if (range.min < range.max)
    {
      result.data = str.data + range.min;
      result.count = range.max - range.min;
    }
  }
  return result;
}

Str8 str8_substring_index(Str8 str, U64 start_index, U64 index_1_after_last)
{
  Str8 sub_str = str8_substring_range(str, range_u64(start_index, index_1_after_last));
  return sub_str;
}

Str8_list str8_split_by_str8(Arena* arena, Str8 str, Str8 sep, Str8_match_flags flags)
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
      Str8 test_sep = str8_substring_range(str, range_u64(char_index, char_index + sep.count));
      if (str8_match(test_sep, sep, flags))
      {
        Str8 sub_str = str8_substring_range(str, range_u64(sub_string_start_index, char_index));
        str8_list_push_str(arena, &list, sub_str);
        sub_string_start_index = char_index + test_sep.count; // Removing the separator as well
        char_index += test_sep.count;
        is_sub_string_left = false;
      }
    }
    if (is_sub_string_left)
    {
        Str8 sub_string = str8_substring_range(str, range_u64(sub_string_start_index, str.count));
        str8_list_push_str(arena, &list, sub_string);
    }
  }
  return list;
}

Str8_list str8_split_by_cstr(Arena* arena, Str8 str, const char* sep, Str8_match_flags flags)
{
  Str8_list list = {};
  Scratch scratch = get_scratch();
  {
    list = str8_split_by_str8(arena, str, str8_from_cstr(sep), flags);
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
                                              Str8FromClit("/"), 
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
                                              Str8FromClit("."), 
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
                                              Str8FromClit("."), 
                                              Str8_match_flag_NONE);
    if (split_list.node_count > 0) 
    {
      result = split_list.last->str;
    }
  }
  end_scratch(&scratch);
  return result;
}

///////////////////////////////////////////////////////////
// Damian: Some extra stuff
//
U64 get_cstr_len(const char* name)
{
  U64 len = 0;
  while (name[len] != '\0') {
    len += 1;
  }
  return len;
}

Data_buffer data_buffer_make(Arena* arena, U64 size)
{
  Data_buffer buffer = {};
  buffer.count = size;
  buffer.data = ArenaPushArr(arena, U8, size);
  return buffer;
}

void str8_printf(const char* fmt, ...)
{
  va_list args = {};
  DefereInitReleaseLoop(va_start(args, fmt), va_end(args))
  DefereInitReleaseLoop(Scratch scratch = get_scratch(), end_scratch(&scratch))
  {
    Str8 formated_str = str8_fmt_format(scratch.arena, fmt, args);
    Str8 formated_str_nt = str8_from_str8_alloc(scratch.arena, formated_str);
    printf("%s", formated_str_nt.data);
  }
}



#endif