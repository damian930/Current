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

// EG: path/str/os/win32/core.h
// Str8 get_file_extension(Str8 path)
// {


//   // Get substring from the back that will be till the last /
//   // Split it using a dot
//   // Get the last substring, return
// }

#endif














