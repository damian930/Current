#ifndef BASE_STRING_H
#define BASE_STRING_H

#include <stdarg.h>
#include "core.h"
#include "math.h"

struct Arena;
// #include "arena.h"

struct Str8 {
  U8* data;
  U64 count;
};
typedef Str8 Data_buffer;

enum Str8_match_flag : U32 {
  Str8_match_flag_NONE            = (1 << 0),
  Str8_match_flag_ignore_case     = (1 << 1),
  Str8_match_flag_normalise_slash = (1 << 2),
};
typedef U32 Str8_match_flags; 

struct Str8_node {
  Str8_node* next;
  Str8_node* prev;
  Str8 str; 
};

struct Str8_list {
  Str8_node* first;
  Str8_node* last;
  U64 node_count;
  U64 char_count;
};

// Damian: Constructors
Str8 str8_empty();
Str8 str8_from_cstr_len(const char* cstr, U64 len);
Str8 str8_from_cstr(const char* cstr);
Str8 str8_from_str8_alloc(Arena* arena, Str8 str8);
#define Str8FromClit(clit) str8_from_cstr_len(clit, ArrayCount(clit) - 1)
// NOTE: str8_from_str8_alloc procduces an array that is null terminated until the next allocation on the same arena

// TODO: Some like this: printf(str8_from_str8_f(arena, "%U32 %Str %csrt"), (U32)x, (Str8)str, "cstr")

// TODO: Mark these here
U8 char_to_lower(U8 ch);
U8 char_to_upper(U8 ch);
U8 normalise_slash(U8 ch);

void str8_list_push_str(Arena* arena, Str8_list* list, Str8 str);
Str8 str8_from_list(Arena* arena, Str8_list* list);

B32 str8_match(Str8 str, Str8 other, Str8_match_flags flags);
B32 str8_match_cstr(Str8 str, const char* c_str, Str8_match_flags flags);

Str8 str8_substring_range(Str8 str, Range_U64 range);                  
Str8 str8_substring_index(Str8 str, U64 start_indexst, U64 index_1_after_last);

Str8_list str8_split_by_str8(Arena* arena, Str8 str, Str8 sep, Str8_match_flags match_flags);

Str8 get_file_basename(Str8 path);
Str8 get_file_name(Str8 path);
Str8 get_file_extension(Str8 path);

// Damian: Some extra stuff
Data_buffer data_buffer_make(Arena* arena, U64 size);
U64 get_cstr_len(const char* name);

#endif














