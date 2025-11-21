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
  grammar           :: (normal_text | fmt)+
  fmt               :: "{" fmt_specifiers "}"
  fmt_specifiers    :: [U8, U16, U32, U64, S8, S16, S32, S64, F32, F64, *P, Cstr, Str8] 
*/

// struct Str8_fmt_node {
//   Str8_fmt_node* next;
//   Str8_fmt_node* prev;
//   Str8 str;
// };

// void str8_fmt(const char* fmt)
// {
//   U64 fmt_size = get_cstr_len(fmt);
//   ForEachEx(i, fmt_size, fmt)
//   {
//     U8 ch = fmt[i];
//     if (ch == '{')
//     {
//       B32 is_fmt_closing = false;
//       U64 fmt_closing_index = 0; 
//       for (U64 j = i; j < fmt_size; j += 1)
//       {
//         if (fmt[j] == '}') 
//         {
//           fmt_closing_index = j;
//           break;
//         }
//       }
//       // Create a node here for the fmt substring for later processing
//     }
//     else
//     {

//     }
//   }


// }

#if 1
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

  // Str8_fmt_token_kind__Pointer_star,
  Str8_fmt_token_kind__Cstr,
  Str8_fmt_token_kind__Str8,

  Str8_fmt_token_kind__END,
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
    // case Str8_fmt_token_kind__Pointer_star: { cstr = "Str8_fmt_token_kind__Pointer_star"; } break;
    case Str8_fmt_token_kind__Cstr: { cstr = "Str8_fmt_token_kind__Cstr"; } break;
    case Str8_fmt_token_kind__Str8: { cstr = "Str8_fmt_token_kind__Str8"; } break;
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
  if (str8_fmt_lexer_is_done(fmt_lexer))
  {
    token.kind = Str8_fmt_token_kind__END;
  }
  else
  {
    fmt_lexer->token_start_index = fmt_lexer->current_index;
    U8 test_char = str8_fmt_lexer_peek_char(fmt_lexer);

    // Just eat tokens until we have found the a new keyword, while we havent then now its just the regular text part 

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
    
    else if (str8_fmt_lexer_match_cstr(fmt_lexer, "{")) { token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__opening_curly_brace); }
    else if (str8_fmt_lexer_match_cstr(fmt_lexer, "}")) { 
      token = str8_fmt_lexer_create_token(fmt_lexer, Str8_fmt_token_kind__closing_curly_brace); }

    else { 
      if (!fmt_lexer->searching_for_closing_brace)
      {
        str8_fmt_lexer_eat_char(fmt_lexer);

        fmt_lexer->searching_for_closing_brace = true;
        U32 lexer_state_current_index = fmt_lexer->current_index;
        U32 lexer_state_token_start_index = fmt_lexer->token_start_index;
        
        B32 next_non_reg_text_token_present = false;
        Str8_fmt_token next_non_reg_text_token = {};
        while (!str8_fmt_lexer_is_done(fmt_lexer))
        {
          Str8_fmt_token test_token = str8_fmt_lexer_eat_next_token(fmt_lexer);
          if (test_token.kind != Str8_fmt_token_kind__regular_text)
          {
            next_non_reg_text_token_present = true;
            next_non_reg_text_token = test_token;
            break;
          }
        } 
        if (next_non_reg_text_token_present)
        {
          // Creating a big text token 
          U64 reg_text_token_end = fmt_lexer->current_index - next_non_reg_text_token.str.count;
          token.kind = Str8_fmt_token_kind__regular_text;
          token.str = str8_substring_index(fmt_lexer->input_str, lexer_state_token_start_index, reg_text_token_end);

          // Manually setting the lexer 1 token behend, so we can also return it next call
          fmt_lexer->current_index -= next_non_reg_text_token.str.count;
        }
        else
        {
          // We are at the end of the lexer and we only got regular text
          token.kind = Str8_fmt_token_kind__regular_text;
          token.str = str8_substring_index(fmt_lexer->input_str, lexer_state_token_start_index, fmt_lexer->current_index);
        }
        fmt_lexer->searching_for_closing_brace = false;
      }
      else {
        str8_fmt_lexer_eat_char(fmt_lexer);
        token.kind = Str8_fmt_token_kind__regular_text;
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


// TEST
void str8_fmt_lex(const char* fmt)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);
  while (!str8_fmt_lexer_is_done(&lexer))
  {
    Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
    printf("Token: %s \n", token_kind_to_cstr(token.kind));
  }

  // END
  Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
  printf("Token: %s \n", token_kind_to_cstr(token.kind));
}


// ------

/* FMT Grammar
  Input        :: " (<fmt_scope>|<regular_text>)* "
  fmt_scope    :: {  U|S|B  }
  regular_text :: alpha_numeric 
  U            :: U8|U16|U32|U64
  S            :: S8|S16|S32|S64
  B            :: B8|B16|B32|B64
*/

enum Str8_fmt_parser_node_kind {
  Str8_fmt_parser_node_kind__regular_text,
  Str8_fmt_parser_node_kind__fmt_scope,
};

enum Str8_fmt_parser_fmt_specifier_kind {
  Str8_fmt_parser_fmt_specifier_kind__U8,
  Str8_fmt_parser_fmt_specifier_kind__U16,
  Str8_fmt_parser_fmt_specifier_kind__U32,
  Str8_fmt_parser_fmt_specifier_kind__U64,
};

struct Str8_fmt_parser_node {
  Str8_fmt_parser_node* next;
  Str8_fmt_parser_node* prev;
  Str8_fmt_parser_fmt_specifier_kind fmt_specifier_kind;
};

struct Str8_fmt_parser_list {
  Str8_fmt_parser_node* first;
  Str8_fmt_parser_node* last;
  U32 node_count;
};

struct Str8_fmt_parser_node_inputs {

};




void str8_fmt_parse(Arena* arena, const char* fmt)
{
  Str8_fmt_lexer lexer = {};
  lexer.input_str = str8_from_cstr(fmt);

  Str8_fmt_parser_node_input* root_node = ArenaPush(arena, Str8_fmt_parser_node_input);
  // root_node->kind = Str8_fmt_parser_node_kind__INPUT;
  
  while (!str8_fmt_lexer_is_done(&lexer))
  {
    Str8_fmt_parser_node* node = str8_fmt_parse_helper(arena, &lexer);
    DllPushBack(root_node, node);
  }

  for (Str8_fmt_parser_node* node = root_node->first; node != 0; node = node->next)
  {
    if (node->kind == Str8_fmt_parser_node_kind__regular_text)
    {
      Str8_fmt_parser_node_regular_text* reg_text_node = &node->u.regular_text;
      printf("Node --> Regular text node %s \n", reg_text_node->text.data);
    }
    if (node->kind == Str8_fmt_parser_node_kind__fmt_scope)
    {
      printf("Node --> fmt \n");

    }
  }
}

void str8_from_str8_f(Arena* arena, const char* fmt, ...)
{
  str8_fmt_parse(arena, fmt);
  

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

  // str8_from_str8_f(arena, "{U32} U8 U16 U32 U64 S8 S16 S32 S64 {F32} F64 * Cstr Str8 ... ");
  str8_from_str8_f(arena, "fasdfasdfasdfasd fafas { fasdfasd }");


}








