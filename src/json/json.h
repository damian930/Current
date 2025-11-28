#ifndef D_JSON_H
#define D_JSON_H

#include "base/include.h"
#include "base/include.cpp" // TODO: Remove this later

/* 
Novemver 28th Json refactor and stuff:
[x] - Move them into h and cpp
[x] - Remove the extra coma at the end of the json thing
[ ] - Remove the todos in the json layer, also then ask the discord if you have handled the errors and just invalid implementation well
[ ] - Add errors to the user 
[ ] - Have a way to create a json object from text
[ ] - Have a way to create a plain json and then fill with data manually
[ ] - Have a funt that then creates json text representation from regular json data you just created 

[ ] - Handle float when parsing 
[ ] - Write a not that only digits and float are usable
[x] - Handle comments

[ ] - Google what /r (/r/n) is to see what maybe other symbold might be used to represent the end of a something in text

*/

enum Json_token_kind {
  Json_token_kind__single_character,  // Damian: This is only used for Json string value when parsing
  Json_token_kind__left_curly_brace,
  Json_token_kind__right_curly_brace,
  Json_token_kind__left_square_brace,
  Json_token_kind__right_square_brace,
  Json_token_kind__double_quotes,
  Json_token_kind__colon,
  Json_token_kind__coma,
  Json_token_kind__true,
  Json_token_kind__false,
  Json_token_kind__null,
  Json_token_kind__number,
  Json_token_kind__END,
};

struct Json_token {
  Json_token_kind kind;
  Str8 str;
};

struct Json_token_node {
  Json_token_node* next;
  Json_token token;
};

struct Json_token_list {
  Json_token_node* first;
  Json_token_node* last;
  U64 count;
};

struct Json_lexer {
  Str8 input_text;
  U64 current_pos;
  U64 token_start_pos;
};

// Lexer stuff
Str8 str8_from_json_token_kind(Json_token_kind kind);
Json_token json_lexer_make_token(Json_lexer* lexer, Json_token_kind token_kind);
B32 json_lexer_is_done(Json_lexer* lexer);
U8 json_lexer_peek_char_from_current(Json_lexer* lexer, U64 n); 
U8 json_lexer_peek_char(Json_lexer* lexer);
U8 json_lexer_eat_char(Json_lexer* lexer);
B32 json_lexer_match_str(Json_lexer* lexer, Str8 str);
Json_token json_lexer_eat_next_token(Json_lexer* lexer);
Json_token json_lexer_peek_next_token(Json_lexer* lexer);
B32 json_lexer_match_next_token(Json_lexer* lexer, Json_token_kind kind);

// ---

// TODO: Move all the structs up top

enum Json_value_kind {
  Json_value_kind__NULL,    // TODO: This name is kind confusing here, especially since json has a null value as well
  Json_value_kind__null,
  Json_value_kind__true,
  Json_value_kind__false,
  Json_value_kind__number,
  Json_value_kind__string,
  Json_value_kind__array,
  Json_value_kind__object,
};

struct Json_value; // Pre decl

struct Json_value_node {
  Json_value_node* next;
  Json_value* value;
};

struct Json_array {
  Json_value_node* first;
  Json_value_node* last;
  U64 count;
};

struct Json_pair {
  Str8 key;
  Json_value* value; 
};

struct Json_pair_node {
  Json_pair_node* next;
  Json_pair pair;
};

struct Json_object {
  Json_pair_node* first;
  Json_pair_node* last;
  U64 count;
};

struct Json_value {
  Json_value_kind kind;
  union {
    S64 number;
    Str8 str;
    Json_array array;
    Json_object object;
  } u;
};

enum Json_error_kind {
  Json_error_kind__NONE = 0,  // This way matching for succ is nice (if (err))
  Json_error_kind__internal,
  Json_error_kind__unclosed_string,

  // --These might just be removed and made into a single error like (Was expecting something else) and then the note will have specifics
  Json_error_kind__object_pair_key_was_expected,
  Json_error_kind__value_was_expected,
  Json_error_kind__unexpected_token_insted_of_a_value, 
  Json_error_kind__unexpected_token_insted_of_a_brace_or_coma_or_colon,  
  // Json_error_kind__unclosed_array_or_coma_value, // Maybe dont a such specific kind, but just use something like "Was expected but got something else"
  // -------

  Json_error_kind__unclosed_object,
};

struct Json_parse_result {
  Json_value* value;
  Json_error_kind error;
  Str8 note;
};

// Parsing
Str8 str8_from_json_error_kind(Json_error_kind kind);
Json_parse_result json_number_value_from_number_token(Arena* arena, Json_lexer* lexer);
Json_parse_result json_parse_string_value(Arena* arena, Json_lexer* lexer);
Json_parse_result json_parse_array_value(Arena* arena, Json_lexer* lexer);
Json_parse_result json_parse_object_value(Arena* arena, Json_lexer* lexer);
Json_parse_result json_parse_value(Arena* arena, Json_lexer* lexer);
Json_parse_result test_create_ast_for_json(Arena* arena, Str8 json_as_str);

// Other
void _json_debug_recreate_json_HELPER(Arena* arena, Json_value* ast, Str8_list* out_json_list);
Str8 json_debug_recreate_json(Arena* arena, Json_value* ast);
void json_debug_print(Json_value* astr);































#endif











