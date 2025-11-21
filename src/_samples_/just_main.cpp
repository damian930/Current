#include "base/include.h"
#include "base/include.cpp"

/*
  Types of strings str8_f supports:
   - %U8, U16, U32, U64
   - %S8, S16, S32, S64
   - %Str8
   - %P
   - %F32, F64
   - %cstr   

  Steps:
   - Lex the input string
   - Parse the string
   - Expand each formatable sub string
   - Create a new string
   - Return
*/

// Go over the string until {, thene find the if there is the closing }. If there is then come back and store the formating into like a node
// Then we will have a list of things that are just regular strings and othe that are to be formated, then we go over the list and formate the things that are to be formated
// Then we jus join the list and we get the thing we needed

/* Str8_fmt grammar
  grammar           :: (normal_text | fmt)*
  fmt               :: "%" fmt_specifiers
  fmt_specifiers    :: [U8, U16, U32, U64, S8, S16, S32, S64, F32, F64, *P, Cstr, Str8] 
*/

#if 1
enum Str8_fmt_token_kind {
  Str8_fmt_token_kind__regular_text,

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

  Str8_fmt_token_kind__END,
};

const char* token_kind_to_cstr(Str8_fmt_token_kind kind)
{
  const char* cstr = "";
  switch (kind)
  {
    case Str8_fmt_token_kind__regular_text: { cstr = "Str8_fmt_token_kind__regular_text"; } break;
    case Str8_fmt_token_kind__fmt_scope_starter: { cstr = "Str8_fmt_token_kind__fmt_scope_starter"; } break;
    case Str8_fmt_token_kind__U8: { cstr = "Str8_fmt_token_kind__U8"; } break;
    case Str8_fmt_token_kind__U16: { cstr = "Str8_fmt_token_kind__U16"; } break;
    case Str8_fmt_token_kind__U32: { cstr = "Str8_fmt_token_kind__U32"; } break;
    case Str8_fmt_token_kind__U64: { cstr = "Str8_fmt_token_kind__U64"; } break;
    case Str8_fmt_token_kind__S8: { cstr = "Str8_fmt_token_kind__S8"; } break;
    case Str8_fmt_token_kind__S16: { cstr = "Str8_fmt_token_kind__S16"; } break;
    case Str8_fmt_token_kind__S32: { cstr = "Str8_fmt_token_kind__S32"; } break;
    case Str8_fmt_token_kind__S64: { cstr = "Str8_fmt_token_kind__S64"; } break;
    case Str8_fmt_token_kind__F32: { cstr = "Str8_fmt_token_kind__F32"; } break;
    case Str8_fmt_token_kind__F64: { cstr = "Str8_fmt_token_kind__F64"; } break;
    case Str8_fmt_token_kind__B:   { cstr = "Str8_fmt_token_kind__B"; } break;
    case Str8_fmt_token_kind__Cstr: { cstr = "Str8_fmt_token_kind__Cstr"; } break;
    case Str8_fmt_token_kind__Str8: { cstr = "Str8_fmt_token_kind__Str8"; } break;
    case Str8_fmt_token_kind__Pointer_star: { cstr = "Str8_fmt_token_kind__Pointer_star"; } break;
    case Str8_fmt_token_kind__END: { cstr = "Str8_fmt_token_kind__END"; } break;
  }
  return cstr;
}

struct Str8_fmt_token {
  Str8 str;
  Str8_fmt_token_kind kind;
};

struct Str8_fmt_lexer {
  Str8 input_str;
  U32 current_index;
  U32 token_start_index;
  B32 searching_for_closing_brace;
};

U8 str8_fmt_lexer_eat_char(Str8_fmt_lexer* fmt_lexer)
{
  Assert(fmt_lexer->current_index != fmt_lexer->input_str.count);
  U8 ch = fmt_lexer->input_str.data[fmt_lexer->current_index++];
  return ch;
}

U8 str8_fmt_lexer_peek_char_from_current(Str8_fmt_lexer* fmt_lexer, U64 index)
{
  Assert(fmt_lexer->current_index + index != fmt_lexer->input_str.count);
  U8 ch = fmt_lexer->input_str.data[fmt_lexer->current_index + index];
  return ch;
}

U8 str8_fmt_lexer_peek_char(Str8_fmt_lexer* fmt_lexer)
{
  U8 ch = str8_fmt_lexer_peek_char_from_current(fmt_lexer, 0);
  return ch;
}

B32 str8_fmt_lexer_is_alive(Str8_fmt_lexer* lexer)
{
  B32 is_alive = (lexer->current_index != lexer->input_str.count);
  return is_alive;
}

U64 str8_fmt_lexer_char_left_to_eat(Str8_fmt_lexer* fmt_lexer)
{
  U64 chars_left = 0;
  if (str8_fmt_lexer_is_alive(fmt_lexer))
  {
    chars_left = fmt_lexer->input_str.count - fmt_lexer->current_index;
  }
  return chars_left;
}

B32 str8_fmt_lexer_match_cstr(Str8_fmt_lexer* fmt_lexer, const char* cstr)
{
  B32 match = true;
  U64 cstr_len = get_cstr_len(cstr);
  if (str8_fmt_lexer_char_left_to_eat(fmt_lexer) >= cstr_len)
  {
    ForEachEx(i, cstr_len, cstr)
    {
      if (str8_fmt_lexer_peek_char_from_current(fmt_lexer, i) != cstr[i])
      {
        match = false;
        break;
      }
    }
  }
  else {
    match = false;
  }
  if (match)
  {
    ForEachEx(i, cstr_len, cstr)
    {
      str8_fmt_lexer_eat_char(fmt_lexer);
    }
  }
  return match;
}

Str8_fmt_token str8_fmt_lexer_create_token(Str8_fmt_lexer* fmt_lexer, Str8_fmt_token_kind kind)
{
  Str8_fmt_token token = {};
  token.str = str8_substring_index(fmt_lexer->input_str, fmt_lexer->token_start_index, fmt_lexer->current_index);
  token.kind = kind;
  return token;
}

B32 str8_fmt_does_char_start_a_token(U8 ch)
{
  B32 result = ((ch == '{') || (ch == '}') || (ch == 'U') || (ch == 'S') || (ch == 'F') || (ch == '*') || (ch == 'C'));
  return result;
}

// Str8_fmt_token str8_fmt_lexer_do_regular_text_token(Str8_fmt_lexer* fmt_lexer)
// {
//   B32 stop = false;
//   while (!stop)
//   {
//     if (   str8_fmt_does_char_start_a_token(str8_fmt_lexer_peek_char(fmt_lexer))
//         || !str8_fmt_lexer_is_alive(fmt_lexer)
//     ) {
//       stop = true;
//     } else {
//       str8_fmt_lexer_eat_char(fmt_lexer);
//     }
//   }
//   Str8_fmt_token token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__regular_text);
//   return token;
// }

Str8_fmt_token str8_fmt_lexer_eat_next_token(Str8_fmt_lexer* lexer)
{
  Str8_fmt_token token = {};
  Assert(!str8_fmt_lexer_is_done(lexer));
  if (!str8_fmt_lexer_is_alive(lexer))
  {
    token.kind = Str8_fmt_token_kind__END;
  }
  else
  {
    lexer->token_start_index = lexer->current_index;
    U8 test_char = str8_fmt_lexer_peek_char(lexer);

    if      (str8_fmt_lexer_match_cstr(lexer, "U8"))  { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__U8);  }
    else if (str8_fmt_lexer_match_cstr(lexer, "U16")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__U16); }
    else if (str8_fmt_lexer_match_cstr(lexer, "U32")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__U32); }
    else if (str8_fmt_lexer_match_cstr(lexer, "U64")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__U64); }
    
    else if (str8_fmt_lexer_match_cstr(lexer, "S8"))  { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__S8);  }
    else if (str8_fmt_lexer_match_cstr(lexer, "S16")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__S16); }
    else if (str8_fmt_lexer_match_cstr(lexer, "S32")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__S32); }
    else if (str8_fmt_lexer_match_cstr(lexer, "S64")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__S64); }
    
    else if (str8_fmt_lexer_match_cstr(lexer, "F32")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__F32); }
    else if (str8_fmt_lexer_match_cstr(lexer, "F64")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__F64); }

    else if (str8_fmt_lexer_match_cstr(lexer, "B"))    { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__B);    }
    else if (str8_fmt_lexer_match_cstr(lexer, "Cstr")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__Cstr); }
    else if (str8_fmt_lexer_match_cstr(lexer, "Str8")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__Str8); }
    else if (str8_fmt_lexer_match_cstr(lexer, "P")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__Pointer_star); }

    else if (str8_fmt_lexer_match_cstr(lexer, "#")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__fmt_scope_starter); }

    else 
    {
      // Eating stuff until we have found a start for the new fmt scope
      while (str8_fmt_lexer_is_alive(lexer) && str8_fmt_lexer_peek_char(lexer) != '#')
      {
        str8_fmt_lexer_eat_char(lexer);
      }
      token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__regular_text);
    }
  }
    
  return token;
}

Str8_fmt_token str8_fmt_lexer_peek_next_token(Str8_fmt_lexer* fmt_lexer)
{
  U64 current_index = fmt_lexer->current_index;
  U64 token_start_index = fmt_lexer->token_start_index;
  Str8_fmt_token token = str8_fmt_lexer_eat_next_token(fmt_lexer);
  // TODO: Save these peecked into a cashe
  fmt_lexer->current_index = current_index;
  fmt_lexer->token_start_index = token_start_index;
  return token;
}

B32 str8_fmt_lexer_match_next_token(Str8_fmt_lexer* lexer, Str8_fmt_token_kind token_kind)
{
  B32 match = false;
  if (str8_fmt_lexer_peek_next_token(lexer).kind == token_kind)
  {
    str8_fmt_lexer_eat_next_token(lexer);
    match = true;
  }
  return match;
}

///////////////////////////////////////////////////////////
// Damian: Parsing
//

enum Str8_fmt_scope_specifier {
  Str8_fmt_scope_specifier__NONE,
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

const char* scpecifier_to_cstr(Str8_fmt_scope_specifier specifier)
{
  const char* cstr = "";
  switch (specifier)
  {
    case Str8_fmt_scope_specifier__NONE: { cstr = "Str8_fmt_scope_specifier__NONE"; } break;
    case Str8_fmt_scope_specifier__U8: { cstr = "Str8_fmt_scope_specifier__U8"; } break;
    case Str8_fmt_scope_specifier__U16: { cstr = "Str8_fmt_scope_specifier__U16"; } break;
    case Str8_fmt_scope_specifier__U32: { cstr = "Str8_fmt_scope_specifier__U32"; } break;
    case Str8_fmt_scope_specifier__U64: { cstr = "Str8_fmt_scope_specifier__U64"; } break;
    case Str8_fmt_scope_specifier__B: { cstr = "Str8_fmt_scope_specifier__B"; } break;
    case Str8_fmt_scope_specifier__Cstr: { cstr = "Str8_fmt_scope_specifier__Cstr"; } break;
    case Str8_fmt_scope_specifier__Str8: { cstr = "Str8_fmt_scope_specifier__Str8"; } break;
    case Str8_fmt_scope_specifier__P: { cstr = "Str8_fmt_scope_specifier__P"; } break;
  }
  return cstr;
}

struct Str8_fmt_scope {
  Str8_fmt_scope* next;
  Str8_fmt_scope* prev;

  Str8 str;
  Str8_fmt_scope_specifier specifier;
};

struct Str8_fmt_scope_list {
  Str8_fmt_scope* first;
  Str8_fmt_scope* last;
  U32 scope_count;
};

Str8_fmt_scope_list* str8_fmt_create_specifier_list(Arena* arena, const char* fmt)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);

  Str8_fmt_scope_list* fmt_list = ArenaPush(arena, Str8_fmt_scope_list);

  while (str8_fmt_lexer_is_alive(&lexer))
  {
    Str8_fmt_scope* new_scope = ArenaPush(arena, Str8_fmt_scope);
    
    Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
    if (token.kind == Str8_fmt_token_kind__regular_text)
    {
      // While we are eating the regular text, just keep appending them 
      new_scope->specifier = Str8_fmt_scope_specifier__NONE;
      new_scope->str = token.str;
    }
    else if (token.kind == Str8_fmt_token_kind__fmt_scope_starter)
    {
      if      (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__U8))  { new_scope->specifier = Str8_fmt_scope_specifier__U8;  }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__U16)) { new_scope->specifier = Str8_fmt_scope_specifier__U16; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__U32)) { new_scope->specifier = Str8_fmt_scope_specifier__U32; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__U64)) { new_scope->specifier = Str8_fmt_scope_specifier__U64; }
      
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__S8))  { new_scope->specifier = Str8_fmt_scope_specifier__S8;  }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__S16)) { new_scope->specifier = Str8_fmt_scope_specifier__S16; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__S32)) { new_scope->specifier = Str8_fmt_scope_specifier__S32; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__S64)) { new_scope->specifier = Str8_fmt_scope_specifier__S64; }
    
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__B))    { new_scope->specifier = Str8_fmt_scope_specifier__B;    }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__Cstr)) { new_scope->specifier = Str8_fmt_scope_specifier__Cstr; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__Str8)) { new_scope->specifier = Str8_fmt_scope_specifier__Str8; }
      else if (str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__Pointer_star)) { new_scope->specifier = Str8_fmt_scope_specifier__P; }
      
      else  
      {
        // Just a scope starter then 
        // Weren't able to get a valid specifier, just regular text then
        new_scope->specifier = Str8_fmt_scope_specifier__NONE;
        new_scope->str = token.str;
      }
    }

    DllPushBack(fmt_list, new_scope);
    fmt_list->scope_count += 1;
  }

  return fmt_list;
}

Str8 str8_from_u64(Arena* arena, U64 u64)
{
  // Getting the number of digits in the u8
  U64 n_digits = 0; 
  if (u64 == 0) {
    n_digits = 1;
  } else {
    for (U64 u64_copy = u64; u64_copy > 0; u64_copy /= 10, n_digits += 1);
  }

  U64 divisor = 1;
  for (U64 i = n_digits; i > 0; i -= 1)
  {
    divisor *= 10;
  }  

  Str8 u8_as_str = data_buffer_make(arena, n_digits);
  for (U64 i = n_digits; i > 0; i -= 1)
  {
    divisor /= 10;
    U64 first_digit = u64 / divisor;
    u64 -= divisor * first_digit;
    u8_as_str.data[n_digits - i] = '0' + (U8)first_digit; 
    Assert(0 <= first_digit && first_digit <= 9);
  }

  return u8_as_str;
}

Str8 str8_from_s64(Arena* arena, S64 s64)
{
  B32 is_negative = (s64 < 0);
  if (is_negative) { s64 *= -1; }
  Str8 u64_as_str = str8_from_u64(arena, (U64)s64);
  Str8 s64_as_str = {};
  if (is_negative)
  {
    s64_as_str = data_buffer_make(arena, u64_as_str.count + 1);
    s64_as_str.data[0] = '-';
    MemCopy(s64_as_str.data + 1, u64_as_str.data, u64_as_str.count);
  }
  else 
  {
    s64_as_str = data_buffer_make(arena, u64_as_str.count);
    MemCopy(s64_as_str.data, u64_as_str.data, u64_as_str.count);
  }
  return s64_as_str;
}

Str8 str8_from_b64(Arena* arena, B64 b64)
{
  Str8 b64_as_str = {};
  if (b64)
  {
    b64_as_str = str8_from_str8_alloc(arena, Str8FromClit("True"));
  }
  else 
  {
    b64_as_str = str8_from_str8_alloc(arena, Str8FromClit("False"));
  }
  return b64_as_str;
}

Str8 str8_from_cstr(Arena* arena, const char* cstr)
{
  Str8 str = str8_from_cstr(cstr);
  return str;
}

Str8 str8_from_u64_hex(Arena* arena, U64 u64)
{
  U64 u64_as_hex_len = 0;
  if (u64 == 0) {
    u64_as_hex_len = 1;
  } else {
    for (U64 u64_copy = u64; u64_copy != 0;)
    {
      U64 quotient  = u64_copy / 16;
      U64 remainder = u64_copy - (quotient * 16);
      u64_copy /= 16;
      u64_as_hex_len += 1;
      Assert(0 <= remainder && remainder <= 15);
    }
  }

  const char* hex_specifier_cstr = "0x";
  U64 hex_specifier_cstr_len = get_cstr_len(hex_specifier_cstr);
  Str8 str8_hex = data_buffer_make(arena, hex_specifier_cstr_len + u64_as_hex_len);
  MemCopy(str8_hex.data, hex_specifier_cstr, hex_specifier_cstr_len);
  for (U32 i = u64_as_hex_len; i > 0; i -= 1)
  {
    U64 quotient  = u64 / 16;
    U64 remainder = u64 - (quotient * 16);
    u64 = quotient;
    U8 hex_digit = '\0';
    switch (remainder)
    {
      case 0:  { hex_digit = '0'; } break;
      case 1:  { hex_digit = '1'; } break;
      case 2:  { hex_digit = '2'; } break;
      case 3:  { hex_digit = '3'; } break;
      case 4:  { hex_digit = '4'; } break;
      case 5:  { hex_digit = '5'; } break;
      case 6:  { hex_digit = '6'; } break;
      case 7:  { hex_digit = '7'; } break;
      case 8:  { hex_digit = '8'; } break;
      case 9:  { hex_digit = '9'; } break;
      case 10: { hex_digit = 'A'; } break;
      case 11: { hex_digit = 'B'; } break;
      case 12: { hex_digit = 'C'; } break;
      case 13: { hex_digit = 'D'; } break;
      case 14: { hex_digit = 'E'; } break;
      case 15: { hex_digit = 'F'; } break;
      default: { InvalidCodePath(); } break;
    }
    str8_hex.data[hex_specifier_cstr_len + i - 1] = hex_digit;
  }

  return str8_hex;
}

Str8 str8_from_p(Arena* arena, void* p)
{
  U64 address = (U64)p;
  Str8 hex_address = str8_from_u64_hex(arena, address);
  return hex_address;
}

Str8 str8_from_str8_f(Arena* arena, const char* fmt, ...)
{
  Str8 result_str = {}; 
  
  va_list args = {};
  DefereLoop(va_start(args, fmt), va_end(args))
  {
    DefereInitReleaseLoop(Scratch scratch = get_scratch(), end_scratch(&scratch))
    {
      Str8_fmt_scope_list* specifier_list = str8_fmt_create_specifier_list(scratch.arena, fmt);
      
      Str8_list* final_str_list = ArenaPush(scratch.arena, Str8_list);  
      for (Str8_fmt_scope* scope = specifier_list->first; scope != 0; scope = scope->next)
      {
        Str8 formated = {};
        if (scope->specifier == Str8_fmt_scope_specifier__NONE)     { formated = scope->str; }
        
        else if (scope->specifier == Str8_fmt_scope_specifier__U8)  { formated = str8_from_u64(scratch.arena, (U64)va_arg(args, U8)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__U16) { formated = str8_from_u64(scratch.arena, (U64)va_arg(args, U16)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__U32) { formated = str8_from_u64(scratch.arena, (U64)va_arg(args, U32)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__U64) { formated = str8_from_u64(scratch.arena, (U64)va_arg(args, U64)); }
        
        else if (scope->specifier == Str8_fmt_scope_specifier__S8)  { formated = str8_from_s64(scratch.arena, (S64)va_arg(args, S8)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__S16) { formated = str8_from_s64(scratch.arena, (S64)va_arg(args, S16)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__S32) { formated = str8_from_s64(scratch.arena, (S64)va_arg(args, S32)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__S64) { formated = str8_from_s64(scratch.arena, (S64)va_arg(args, S64)); }
        
        // TODO: This might create invalid offsets if i pass in a B8 for example, FIT THIS.
        else if (scope->specifier == Str8_fmt_scope_specifier__B)    { formated = str8_from_b64(scratch.arena, va_arg(args, B64)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__Cstr) { formated = str8_from_cstr(scratch.arena, va_arg(args, const char*)); }
        else if (scope->specifier == Str8_fmt_scope_specifier__Str8) { formated = va_arg(args, Str8); }
        else if (scope->specifier == Str8_fmt_scope_specifier__P)    { formated = str8_from_p(scratch.arena, va_arg(args, void*)); }
        
        else {
          InvalidCodePath();
        }
        str8_list_push_str(scratch.arena, final_str_list, formated);
      }
      
      // Pushing the final String onto the passed in arena
      result_str = str8_from_list(arena, final_str_list);
    }
  }

  return result_str;
}

#endif


// --------

void EntryPoint();

int main()
{
  DefereLoop(os_win32_state_init(), os_win32_state_release())
  {
    EntryPoint();
  }

  return 0;
}

void EntryPoint()
{
  Arena* arena = arena_alloc(Kilobytes_U64(10), "Main arena");

  Str8 str = {};
  
  str = str8_from_str8_f(arena, "{U32} U8 U16 U32 U64 S8 S16 S32 S64 {F32} F64 * Cstr Str8 ... ");
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#U8 #U16 #U32 #U64", 254, 15000, 10000000, 99999999999);
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#S8 #S16 #S32 #S64", -254, 15000, -10000000, 99999999999);
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#B #B #B", 1, 0, -1);
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#Cstr #Cstr", "", "Flopper");
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#Str8 #Str8", Str8FromClit(""), Str8FromClit("Str8_Flopper"));
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#Str8 #Str8", Str8FromClit(""), Str8FromClit("Str8_Flopper"));
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#P", (void*)0);
  printf("%s \n", str.data);
  
  str = str8_from_str8_f(arena, "#P", arena);
  printf("%s \n", str.data);




}








