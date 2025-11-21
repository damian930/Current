#ifndef D_STRING_FMT_H
#define D_STRING_FMT_H

#include "string.h"

enum Str8_fmt_token_kind {
  Str8_fmt_token_kind__regular_text,
  Str8_fmt_token_kind__integer_value,

  Str8_fmt_token_kind__fmt_scope_starter,

  Str8_fmt_token_kind__U8,
  Str8_fmt_token_kind__U16,
  Str8_fmt_token_kind__U32,
  Str8_fmt_token_kind__U64,

  Str8_fmt_token_kind__S8,
  Str8_fmt_token_kind__S16,
  Str8_fmt_token_kind__S32,
  Str8_fmt_token_kind__S64,

  Str8_fmt_token_kind__F32,
  Str8_fmt_token_kind__F64,

  Str8_fmt_token_kind__B,

  Str8_fmt_token_kind__Cstr,
  Str8_fmt_token_kind__Str8,
  Str8_fmt_token_kind__Pointer_star,

  Str8_fmt_token_kind__align_right_flag,

  Str8_fmt_token_kind__END,
};

struct Str8_fmt_token {
  Str8 str;
  Str8_fmt_token_kind kind;
};

struct Str8_fmt_lexer {
  Str8 input_str;
  U64 current_index;
  U64 token_start_index;
  B32 searching_for_closing_brace;
};

enum Str8_fmt_scope_specifier {
  Str8_fmt_scope_specifier__regular_text,
  Str8_fmt_scope_specifier__U8,
  Str8_fmt_scope_specifier__U16,
  Str8_fmt_scope_specifier__U32,
  Str8_fmt_scope_specifier__U64,

  Str8_fmt_scope_specifier__S8,
  Str8_fmt_scope_specifier__S16,
  Str8_fmt_scope_specifier__S32,
  Str8_fmt_scope_specifier__S64,

  Str8_fmt_scope_specifier__B,
  Str8_fmt_scope_specifier__Cstr,
  Str8_fmt_scope_specifier__Str8,
  Str8_fmt_scope_specifier__P,
};

enum Str8_fmt_scope_flag {
  Str8_fmt_scope_flag__NONE,
  Str8_fmt_scope_flag__align_left,
};

struct Str8_fmt_scope {
  Str8_fmt_scope* next;
  Str8_fmt_scope* prev;

  Str8_fmt_scope_specifier specifier;
  Str8 str;

  Str8_fmt_scope_flag flag;
  F32 value_for_the_flag;
};

struct Str8_fmt_scope_list {
  Str8_fmt_scope* first;
  Str8_fmt_scope* last;
  U32 scope_count;
};

// Lexer stuff
B32 str8_fmt_lexer_is_alive(Str8_fmt_lexer* lexer);

U8 str8_fmt_lexer_eat_char(Str8_fmt_lexer* fmt_lexer);
U8 str8_fmt_lexer_peek_char_from_current(Str8_fmt_lexer* fmt_lexer, U64 index);
U8 str8_fmt_lexer_peek_char(Str8_fmt_lexer* fmt_lexer);
U64 str8_fmt_lexer_char_left_to_eat(Str8_fmt_lexer* fmt_lexer);

B32 str8_fmt_lexer_match_cstr(Str8_fmt_lexer* fmt_lexer, const char* cstr);
Str8_fmt_token str8_fmt_lexer_create_token(Str8_fmt_lexer* fmt_lexer, Str8_fmt_token_kind kind);
Str8_fmt_token str8_fmt_lexer_eat_next_token(Str8_fmt_lexer* lexer);
Str8_fmt_token str8_fmt_lexer_peek_next_token(Str8_fmt_lexer* fmt_lexer);
B32 str8_fmt_lexer_match_next_token(Str8_fmt_lexer* lexer, Str8_fmt_token_kind token_kind);

// Formater
Str8_fmt_scope_list* str8_fmt_create_specifier_list(Arena* arena, const char* fmt);
Str8 str8_fmt_format(Arena* arena, const char* fmt, va_list args);

// Converters
Str8 str8_from_u64(Arena* arena, U64 u64);
Str8 str8_from_s64(Arena* arena, S64 s64);
Str8 str8_from_b64(Arena* arena, B64 b64);
Str8 str8_from_u64_hex(Arena* arena, U64 u64);
Str8 str8_from_p(Arena* arena, void* p);
Str8 str8_from_str8_f(Arena* arena, const char* fmt, ...);

U64 u64_from_str8(Str8 str)
{
  // TODO:
  return 3;
}












#endif

















