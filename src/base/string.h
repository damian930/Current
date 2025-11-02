#ifndef BASE_STRING_H
#define BASE_STRING_H

#include "core.h"

struct Arena;
// #include "arena.h"

struct Str8 {
  U8* data;
  U64 count;
};

typedef Str8 Data_buffer;
Data_buffer data_buffer_make(Arena* arena, U64 size);

U64 cstr_len(const char* name);

Str8 str8_from_cstr_len(Arena* arena, const char* cstr, U64 len);
Str8 str8_from_cstr(Arena* arena, const char* cstr);
Str8 str8_from_str8(Arena* arena, Str8 str8);

Str8 str8_from_cstr_len_temp_null_term(Arena* arena, const char* cstr, U64 len);
Str8 str8_from_cstr_temp_null_term(Arena* arena, const char* cstr);
Str8 str8_from_str8_temp_null_term(Arena* arena, Str8 str8);

#define Str8FromClit(arena_p, clit)              str8_from_cstr_len(arena_p, clit, ArrayCount(clit) - 1)
#define Str8FromClit_TempNullTerm(arena_p, clit) str8_from_cstr_len_temp_null_term(arena_p, clit, ArrayCount(clit) - 1)

///////////////////////////////////////////////////////////
// Damian: THIS IS NEW CODE, SO THIS IS SEPARATED, KIND DEBUG
//
enum Str8_match_flags : U32 {
  Str8_match_flag_NONE            = (1 << 0),
  Str8_match_flag_ignore_case     = (1 << 1),
  Str8_match_flag_normalise_slash = (1 << 2),
};

struct Str8_node {
  Str8_node* next;
  Str8_node* prev;
  Str8 str; 
};

struct Str8_list {
  Str8_node* first;
  Str8_node* last;
  U64 count;
};

U8 char_to_lower(U8 ch);
U8 char_to_upper(U8 ch);
U8 normalise_slash(U8 ch);

void str8_list_push_str(Arena* arena, Str8_list* list, Str8 str);

B32 str8_match(Str8 str, Str8 other, U32 flags);
B32 str8_match_cstr(Str8 str, const char* c_str, U32 flags);

Str8 str8_substring(Str8 str, U64 start_index, U64 end_index);
Str8_list str8_split_by_str8(Arena* arena, Str8 str, Str8 sep, U32 flags);

Str8 get_file_basename(Str8 path);
Str8 get_file_name(Str8 path);
Str8 get_file_extension(Str8 path);




#endif














