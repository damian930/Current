#ifndef D_STRING_FMT_CPP
#define D_STRING_FMT_CPP

#include "string_fmt.h"
#include "arena.cpp"

///////////////////////////////////////////////////////////
// Damian: Lexer stuff
//

U8 str8_fmt_lexer_eat_char(Str8_fmt_lexer* fmt_lexer)
{
  // TODO: What about just returning /0 if we are at the end of the lexer here ???
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

Str8_fmt_token str8_fmt_lexer_eat_next_token(Str8_fmt_lexer* lexer)
{
  Str8_fmt_token token = {};
  Assert(str8_fmt_lexer_is_alive(lexer));
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

    else if (str8_fmt_lexer_match_cstr(lexer, ":<")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__align_right_flag); }

    else if (str8_fmt_lexer_match_cstr(lexer, "#")) { token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__fmt_scope_starter); }

    else 
    {
      // Integer numbers
      if (is_char_a_number(test_char))
      {
        str8_fmt_lexer_eat_char(lexer); // Damian: I know this is extra code, but i just want this to be clear here
        while (str8_fmt_lexer_is_alive(lexer) && is_char_a_number(str8_fmt_lexer_peek_char(lexer)))
        {
          str8_fmt_lexer_eat_char(lexer);
        }
        token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__integer_value);
      }
      else // No match 
      {
        str8_fmt_lexer_eat_char(lexer);
        token = str8_fmt_lexer_create_token(lexer, Str8_fmt_token_kind__Other_char);
      }
    }

  }
  return token;
}

Str8_fmt_token str8_fmt_lexer_peek_token_from_current(Str8_fmt_lexer* lexer, U32 token_from_current_index)
{
  U64 current_index = lexer->current_index;
  U64 token_start_index = lexer->token_start_index;
  for (U32 i = 0; i < token_from_current_index; i += 1) { 
    str8_fmt_lexer_eat_next_token(lexer);
  }
  Str8_fmt_token token = str8_fmt_lexer_eat_next_token(lexer);
  // TODO: Save these peecked into a cashe
  lexer->current_index = current_index;
  lexer->token_start_index = token_start_index;
  return token;
}

// TODO: If the func about stays, then use it inside of this one
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
// Damian: Formater
//
Str8_fmt_scope_list* str8_fmt_create_specifier_list(Arena* arena, const char* fmt)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);

  Str8_fmt_scope_list* fmt_list = ArenaPush(arena, Str8_fmt_scope_list);

  while (str8_fmt_lexer_is_alive(&lexer))
  {
    Str8_fmt_scope* new_scope = ArenaPush(arena, Str8_fmt_scope);
    
    Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
    if (token.kind == Str8_fmt_token_kind__Other_char)
    {
      U64 lexer_start_index = lexer.token_start_index;
      while (   str8_fmt_lexer_is_alive(&lexer) 
             && str8_fmt_lexer_match_next_token(&lexer, Str8_fmt_token_kind__Other_char)
      ) {}
      new_scope->specifier = Str8_fmt_scope_specifier__regular_text;
      new_scope->str = str8_substring_index(lexer.input_str, lexer_start_index, lexer.current_index); 
    }
    else if (token.kind == Str8_fmt_token_kind__fmt_scope_starter)
    {
      B32 specifier_found = true;
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
        // No specifier was found, so this means that the scope starter is just regular text
        specifier_found = false;
        new_scope->specifier = Str8_fmt_scope_specifier__regular_text;
        new_scope->str = token.str;
      }

      if (specifier_found)
      {
        if (   str8_fmt_lexer_peek_token_from_current(&lexer, 0).kind == Str8_fmt_token_kind__align_right_flag
            && str8_fmt_lexer_peek_token_from_current(&lexer, 1).kind == Str8_fmt_token_kind__integer_value
        ) {
          str8_fmt_lexer_eat_next_token(&lexer);
          Str8_fmt_token align_value_token = str8_fmt_lexer_eat_next_token(&lexer);
          U64 align_value = u64_from_str8(align_value_token.str);

          new_scope->flag = Str8_fmt_scope_flag__align_left;
          new_scope->value_for_the_flag = (F32)align_value;
        }
      }

    }

    DllPushBack(fmt_list, new_scope);
    fmt_list->scope_count += 1;
  }

  return fmt_list;
}

Str8 str8_fmt_format(Arena* arena, const char* fmt, va_list args)
{
  Str8 result_str = {}; 
  DefereInitReleaseLoop(Scratch scratch = get_scratch(), end_scratch(&scratch))
  {
    Str8_fmt_scope_list* specifier_list = str8_fmt_create_specifier_list(scratch.arena, fmt);
    
    Str8_list* final_str_list = ArenaPush(scratch.arena, Str8_list);  
    for (Str8_fmt_scope* scope = specifier_list->first; scope != 0; scope = scope->next)
    {
      Str8 formated = {};
      if (scope->specifier == Str8_fmt_scope_specifier__regular_text) 
      { 
        // TODO: Go until the fist real token is found. Then just make a substring in range from start of lexer to the current thing 
        formated = scope->str; 
      }
      
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
      else if (scope->specifier == Str8_fmt_scope_specifier__Cstr) { formated = str8_from_cstr(va_arg(args, const char*)); }
      else if (scope->specifier == Str8_fmt_scope_specifier__Str8) { formated = va_arg(args, Str8); }
      else if (scope->specifier == Str8_fmt_scope_specifier__P)    { formated = str8_from_p(scratch.arena, va_arg(args, void*)); }
      
      else {
        InvalidCodePath();
      }

      if (scope->flag == Str8_fmt_scope_flag__align_left)
      {
        U64 align_value = (U64)scope->value_for_the_flag;
        U64 chars_needed_to_align = 0;
        if (align_value > formated.count)
        {
          chars_needed_to_align = align_value - formated.count;
        }
        Str8 align_str = data_buffer_make(scratch.arena, chars_needed_to_align);
        ForEachEx(i, align_str.count, align_str.data)
        {
          align_str.data[i] = ' '; // Sep
        }
        str8_list_push_str(scratch.arena, final_str_list, formated);
        str8_list_push_str(scratch.arena, final_str_list, align_str);
      }
      else 
      {
        str8_list_push_str(scratch.arena, final_str_list, formated);
      }
    }
    
    // Pushing the final String onto the passed in arena
    result_str = str8_from_list(arena, final_str_list);
  }
  return result_str;
}

///////////////////////////////////////////////////////////
// Damian: Converters
//
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
  for (U64 i = u64_as_hex_len; i > 0; i -= 1)
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
        if (scope->specifier == Str8_fmt_scope_specifier__regular_text)     { formated = scope->str; }
        
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
        else if (scope->specifier == Str8_fmt_scope_specifier__Cstr) { formated = str8_from_cstr(va_arg(args, const char*)); }
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