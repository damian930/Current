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
enum Str8_fmt_token_kind {
  Str8_fmt_token_kind__regular_text,

  Str8_fmt_token_kind__opening_curly_brace,
  Str8_fmt_token_kind__closing_curly_brace,

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

  Str8_fmt_token_kind__Pointer_star,
  Str8_fmt_token_kind__Cstr,
  Str8_fmt_token_kind__Str8,
};

const char* token_kind_to_cstr(Str8_fmt_token_kind kind)
{
  const char* cstr = "";
  switch (kind)
  {
    case Str8_fmt_token_kind__regular_text: { cstr = "Str8_fmt_token_kind__regular_text"; } break;
    case Str8_fmt_token_kind__opening_curly_brace: { cstr = "Str8_fmt_token_kind__opening_curly_brace"; } break;
    case Str8_fmt_token_kind__closing_curly_brace: { cstr = "Str8_fmt_token_kind__closing_curly_brace"; } break;
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
    case Str8_fmt_token_kind__Pointer_star: { cstr = "Str8_fmt_token_kind__Pointer_star"; } break;
    case Str8_fmt_token_kind__Cstr: { cstr = "Str8_fmt_token_kind__Cstr"; } break;
    case Str8_fmt_token_kind__Str8: { cstr = "Str8_fmt_token_kind__Str8"; } break;
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

B32 str8_fmt_lexer_is_done(Str8_fmt_lexer* lexer)
{
  B32 is_done = (lexer->current_index == lexer->input_str.count);
  return is_done;
}

U64 str8_fmt_lexer_char_left_to_eat(Str8_fmt_lexer* fmt_lexer)
{
  U64 chars_left = 0;
  if (!str8_fmt_lexer_is_done(fmt_lexer))
  {
    chars_left = fmt_lexer->input_str.count - fmt_lexer->current_index + 1;
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
  if (match)
  {
    ForEachEx(i, cstr_len, cstr)
    {
      str8_fmt_lexer_eat_char(fmt_lexer);
    }
  }
  return match;
}

// B32 str8_fmt_lexer_match_char(Str8_fmt_lexer* fmt_lexer, U8 ch)
// {
//   B32 match = (str8_fmt_lexer_peek_char(fmt_lexer) == ch);
//   return match;
// }

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

Str8_fmt_token str8_fmt_lexer_do_regular_text_token(Str8_fmt_lexer* fmt_lexer)
{
  B32 stop = false;
  while (!stop)
  {
    if (   str8_fmt_does_char_start_a_token(str8_fmt_lexer_peek_char(fmt_lexer))
        || str8_fmt_lexer_is_done(fmt_lexer)
    ) {
      stop = true;
    } else {
      str8_fmt_lexer_eat_char(fmt_lexer);
    }
  }
  Str8_fmt_token token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__regular_text);
  return token;
}

Str8_fmt_token str8_fmt_lexer_eat_next_token(Str8_fmt_lexer* fmt_lexer)
{
  Str8_fmt_token token = {};
  Assert(!str8_fmt_lexer_is_done(fmt_lexer));
  if (!str8_fmt_lexer_is_done(fmt_lexer))
  {
    fmt_lexer->token_start_index = fmt_lexer->current_index;
    U8 test_char = str8_fmt_lexer_peek_char(fmt_lexer);
  
    switch (test_char)
    {
      default: 
      { 
        if      (str8_fmt_lexer_match_cstr(fmt_lexer, "U8"))  { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__U8);  }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "U16")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__U16); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "U32")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__U32); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "U64")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__U64); }
        
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "S8"))  { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__S8);  }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "S16")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__S16); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "S32")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__S32); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "S64")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__S64); }
        
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "F32")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__F32); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "F64")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__F64); }
        
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "Cstr")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__Cstr); }
        else if (str8_fmt_lexer_match_cstr(fmt_lexer, "Str8")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__Str8); }
        
        else { token = str8_fmt_lexer_do_regular_text_token(fmt_lexer); }
      } break;
  
      case '{':
      {
        str8_fmt_lexer_eat_char(fmt_lexer);
        token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__opening_curly_brace);
      } break;

      case '}':
      {
        str8_fmt_lexer_eat_char(fmt_lexer);
        token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__closing_curly_brace);
      } break;

      case '*':
      {
        str8_fmt_lexer_eat_char(fmt_lexer);
        token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__Pointer_star);
      }
     
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

// ------

enum Str8_fmt_ast_node_kind {
  Str8_fmt_ast_node_kind__
};

struct Str8_fmt_list_node {
  Str8_fmt_list_node* next;
  Str8_fmt_list_node* prev;
  Str8_fmt_token token;
};

struct Str8_fmt_list {
  Str8_fmt_list_node* first;
  Str8_fmt_list_node* last;
  U32 count;
};

void str8_fmt_parse_fmt_scope(Str8_fmt_lexer* fmt_lexer)
{
  Str8_fmt_token token = str8_fmt_lexer_eat_next_token(fmt_lexer);
  Assert(token.kind == Str8_fmt_token_kind__opening_curly_brace);
  if (token.kind == Str8_fmt_token_kind__opening_curly_brace)
  {
    while ((token = str8_fmt_lexer_eat_next_token(fmt_lexer)),
           (token.kind != Str8_fmt_token_kind__closing_curly_brace 
           && !str8_fmt_lexer_is_done(fmt_lexer))
    ) {
      printf("Token kind: %s \n", token_kind_to_cstr(token.kind));
      
    }

  }

  // TODO: Eat the { here

  // Create a node for the tree, go over the tokens until the }, 
  // when done, if there is only 1 token between then and its a tokene than can be formated, 
  // then create a node for that with the type, else just make the new node be texture node
  // and fill with string data from the tokens    
}

void str8_fmt_parse(const char* fmt)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);

  // TODO: Think if i want the lexing next token api to return a token that represents that i am an the end of the lexer
  //       or if i want to manually if the lexer for being consumed at the caller site

  while (!str8_fmt_lexer_is_done(&lexer))
  {
    Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
    if (token.kind == Str8_fmt_token_kind__opening_curly_brace)
    {
      Str8_fmt_token format_specifier = str8_fmt_lexer_peek_next_token(&lexer);
      if (format_specifier.kind == )
      
    }

  }

}


void str8_from_str8_f(Arena* arena, const char* fmt, ...)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);
  str8_fmt_parse(fmt);
  

  // va_list args = {};
  // DefereLoop(va_start(args, fmt), va_end(args))
  // {

    // Lex the thing
    // Parse the thing
    // Do the va_arg stuff and format the string

    // va_arg(args, Type_to_access)
  // }

  // Iterate over the passed in string
  // va_arg(args, Type);
}


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

  str8_from_str8_f(arena, "{U32} U8 U16 U32 U64 S8 S16 S32 S64 {F32} F64 * Cstr Str8 ... ");
  str8_from_str8_f(arena, "");

}








