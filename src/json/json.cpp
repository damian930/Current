#ifndef D_JSON_CPP
#define D_JSON_CPP

#include "json.h"

///////////////////////////////////////////////////////////
// Damian: Lexer stuff
//
Str8 str8_from_json_token_kind(Json_token_kind kind)
{
  Str8 result = {};
  switch (kind)
  {
    default: {} break;
    case Json_token_kind__single_character:   { Str8FromClit("Json_token_kind__single_character");   } break;  
    case Json_token_kind__left_curly_brace:   { Str8FromClit("Json_token_kind__left_curly_brace");   } break;
    case Json_token_kind__right_curly_brace:  { Str8FromClit("Json_token_kind__right_curly_brace");  } break;
    case Json_token_kind__left_square_brace:  { Str8FromClit("Json_token_kind__left_square_brace");  } break;
    case Json_token_kind__right_square_brace: { Str8FromClit("Json_token_kind__right_square_brace"); } break;
    case Json_token_kind__double_quotes:      { Str8FromClit("Json_token_kind__double_quotes");      } break;
    case Json_token_kind__colon:              { Str8FromClit("Json_token_kind__colon");              } break;
    case Json_token_kind__coma:               { Str8FromClit("Json_token_kind__coma");               } break;
    case Json_token_kind__true:               { Str8FromClit("Json_token_kind__true");               } break;
    case Json_token_kind__false:              { Str8FromClit("Json_token_kind__false");              } break;
    case Json_token_kind__null:               { Str8FromClit("Json_token_kind__null");               } break;
    case Json_token_kind__number:             { Str8FromClit("Json_token_kind__number");             } break;
    case Json_token_kind__END:                { Str8FromClit("Json_token_kind__END");                } break;
  }
  return result;
}

Json_token json_lexer_make_token(Json_lexer* lexer, Json_token_kind token_kind)
{
  Json_token token = {};
  token.kind = token_kind;
  token.str = str8_substring_index(lexer->input_text, lexer->token_start_pos, lexer->current_pos);

  return token;
}

B32 json_lexer_is_done(Json_lexer* lexer)
{
  B32 is_done = (lexer->current_pos == lexer->input_text.count);
  return is_done;
}

U8 json_lexer_peek_char_from_current(Json_lexer* lexer, U64 n)
{
  U8 ch = '\0';
  if (lexer->current_pos + n < lexer->input_text.count)
  {
    ch = lexer->input_text.data[lexer->current_pos + n];
  }
  else
  {
    Assert(false);
  }
  return ch;
}

U8 json_lexer_peek_char(Json_lexer* lexer)
{
  U8 ch = json_lexer_peek_char_from_current(lexer, 0);
  return ch;
}

U8 json_lexer_eat_char(Json_lexer* lexer)
{
  U8 ch = '\0';
  ch = json_lexer_peek_char(lexer);
  lexer->current_pos += 1;
  return ch;
}

B32 json_lexer_match_str(Json_lexer* lexer, Str8 str)
{
  B32 is_match = true;
  if (lexer->current_pos + str.count < lexer->input_text.count)
  {
    U64 safe_point = lexer->current_pos;
    for (U64 i = 0; i < str.count; i += 1)
    {
      if (json_lexer_eat_char(lexer) != str.data[i])
      {
        lexer->current_pos = safe_point;
        is_match = false;
        break;
      }
    }
  }
  else 
  {
    is_match = false;
  }
  return is_match;
}

Json_token json_lexer_eat_next_token(Json_lexer* lexer)
{
  Json_token result_token = {};
  if (json_lexer_is_done(lexer))
  {
    result_token = json_lexer_make_token(lexer, Json_token_kind__END);
  }
  else
  {
    // Here we deal with all the things we need to skip (comments and spaces)
    for (;!json_lexer_is_done(lexer);)
    {
      if (   json_lexer_peek_char(lexer) == ' '  // Spaces 
          || json_lexer_peek_char(lexer) == '\n'
          || json_lexer_peek_char(lexer) == '\r'
      ) {
        json_lexer_eat_char(lexer);
      }
      else if (json_lexer_match_str(lexer, Str8FromClit("//"))) // Single line comments
      {
        for (;!json_lexer_is_done(lexer);)
        {
          U8 ch = json_lexer_eat_char(lexer);
          if (ch == '\n') 
          {
            break;
          }
        }
      }
      else
      {
        break; // Nothin to skip no more here
      }
    }

    if (json_lexer_is_done(lexer))
    {
      result_token = json_lexer_make_token(lexer, Json_token_kind__END);
    }
    else
    {
      lexer->token_start_pos = lexer->current_pos;
      U8 first_char = json_lexer_eat_char(lexer); 
      
      switch (first_char)
      {
        case ':': { result_token = json_lexer_make_token(lexer, Json_token_kind__colon); } break;
        case ',': { result_token = json_lexer_make_token(lexer, Json_token_kind__coma);  } break;
        case '[': { result_token = json_lexer_make_token(lexer, Json_token_kind__left_square_brace); } break;
        case ']': { result_token = json_lexer_make_token(lexer, Json_token_kind__right_square_brace); } break;
        case '{': { result_token = json_lexer_make_token(lexer, Json_token_kind__left_curly_brace); } break;
        case '}': { result_token = json_lexer_make_token(lexer, Json_token_kind__right_curly_brace); } break;
        case '"': { result_token = json_lexer_make_token(lexer, Json_token_kind__double_quotes); } break;
        default: 
        {
          B32 is_token_created = false;
          if (!is_token_created && first_char == 'f' && json_lexer_match_str(lexer, Str8FromClit("alse"))) 
          {
            result_token = json_lexer_make_token(lexer, Json_token_kind__false);
            is_token_created = true;
          }
          
          if (!is_token_created && first_char == 't' && json_lexer_match_str(lexer, Str8FromClit("rue")))
          {
            result_token = json_lexer_make_token(lexer, Json_token_kind__true);
            is_token_created = true;
          } 
          
          if (!is_token_created && first_char == 'n' && json_lexer_match_str(lexer, Str8FromClit("ull")))
          {
            result_token = json_lexer_make_token(lexer, Json_token_kind__null);
            is_token_created = true;
          }
          
          // Damian: String are not handled here, event tho i can, cause i then want to have to parse them,
          //         thne i will be able to have more context for errors and errors all will be in the 
          //         parsing level.
          
          // TODO: Handle float as well
          if (!is_token_created 
              && (is_char_a_number(first_char) || first_char == '-' || first_char == '+')
          ) {
            // Not doing anything to + and - since we have already consumed them 
            while (!json_lexer_is_done(lexer) && is_char_a_number(json_lexer_peek_char(lexer)))
            {
              json_lexer_eat_char(lexer);
            }
            result_token = json_lexer_make_token(lexer, Json_token_kind__number);
            is_token_created = true;
          }
          
          if (!is_token_created)
          {
            result_token = json_lexer_make_token(lexer, Json_token_kind__single_character);
          }
        } break;
      }
    }
  }

  // This is here to make sure that peeking next tokens work the way it is supposed to.
  // Without this, after eating a token, we will be getting the same token we just ate, 
  // because eat_next_token only sets the valid positions for lexer before eating,
  // this means that after eating lexer start_token_pos is the pos for the token we just ate, 
  // and not the next token we want to peek. This fixes that.
  lexer->token_start_pos = lexer->current_pos;

  return result_token;
}

Json_token json_lexer_peek_next_token(Json_lexer* lexer)
{
  U64 current_pos = lexer->current_pos;
  U64 token_start_pos = lexer->token_start_pos;
  Json_token token = json_lexer_eat_next_token(lexer);
  lexer->current_pos = current_pos;
  lexer->token_start_pos = token_start_pos;
  return token;
}

B32 json_lexer_match_next_token(Json_lexer* lexer, Json_token_kind kind)
{
  B32 is_matched = (json_lexer_peek_next_token(lexer).kind == kind);
  if (is_matched)
  {
    Json_token _var_for_assert_ = json_lexer_eat_next_token(lexer);
    Assert(_var_for_assert_.kind == kind);
  }
  return is_matched;
}

///////////////////////////////////////////////////////////
// Damian: Parsing
//
// Str8 str8_from_json_error_kind(Json_error_kind kind)
// {
//   Str8 result = {};
//   switch (kind)
//   {
//     default: { Assert(false); };
//     case Json_error_kind__NONE:            { result = Str8FromClit("Json_error_kind__NONE");            } break;
//     case Json_error_kind__internal:        { result = Str8FromClit("Json_error_kind__internal");        } break;
//     case Json_error_kind__unclosed_string: { result = Str8FromClit("Json_error_kind__unclosed_string"); } break;
//     case Json_error_kind__unclosed_array:  { result = Str8FromClit("Json_error_kind__unclosed_array");  } break;
//     case Json_error_kind__unclosed_object: { result = Str8FromClit("Json_error_kind__unclosed_object"); } break;
//   }
//   return result;
// }

Json_parse_result json_number_value_from_number_token(Arena* arena, Json_lexer* lexer)
{
  Json_parse_result parse_result    = {};
  parse_result.value                = ArenaPush(arena, Json_value);
  parse_result.value->kind          = Json_value_kind__number; 
  
  // Aliases for easier usage 
  Json_value* parse_value           = parse_result.value;
  Json_error_kind* parse_error_kind = &parse_result.error;
  Str8* parse_note                  = &parse_result.note;

  Json_token number_token = json_lexer_eat_next_token(lexer);
  if (number_token.kind != Json_token_kind__number)
  {
    *parse_error_kind = Json_error_kind__internal;
    *parse_note = str8_from_list_v(arena, 8, 
                                   Str8FromClit("Error(Internal): \n"),
                                   Str8FromClit(" -\""),
                                   Str8FromClit(__func__),
                                   Str8FromClit("\""),
                                   Str8FromClit(" was called by the parsing system, but the first token didnt match.\n"),
                                   Str8FromClit(" -Json_token_kind__number was expected, but token kind "),
                                   str8_from_json_token_kind(number_token.kind),
                                   Str8FromClit(" was matched insted.")
    );
  }
  else
  {
    S64 value = s64_from_str8(number_token.str);
    parse_value->u.number = value;
  }

  return parse_result;
} 

Json_parse_result json_parse_string_value(Arena* arena, Json_lexer* lexer)
{
  Json_parse_result parse_result    = {};
  parse_result.value                = ArenaPush(arena, Json_value);
  parse_result.value->kind          = Json_value_kind__string; 
  
  // Aliases for easier usage 
  Json_value* parse_value           = parse_result.value;
  Json_error_kind* parse_error_kind = &parse_result.error;
  Str8* parse_note                  = &parse_result.note;

  Json_token double_quotes_token = json_lexer_eat_next_token(lexer);
  if (double_quotes_token.kind != Json_token_kind__double_quotes)
  {
    *parse_error_kind = Json_error_kind__internal;
    *parse_note = str8_from_list_v(arena, 8, 
                                   Str8FromClit("Error(Internal): \n"),
                                   Str8FromClit(" -\""),
                                   Str8FromClit(__func__),
                                   Str8FromClit("\""),
                                   Str8FromClit(" was called by the parsing system, but the first token didnt match.\n"),
                                   Str8FromClit(" -Json_token_kind__double_quotes was expected, but token kind "),
                                   str8_from_json_token_kind(double_quotes_token.kind),
                                   Str8FromClit(" was matched insted.")
    );
  }
  else // Scope that handles valid case 
  {
    DefereInitReleaseLoop(Scratch scratch = get_scratch(&arena, 1), end_scratch(&scratch))
    {
      B32 is_early_exit = false;
      Str8_list* str_list = ArenaPush(scratch.arena, Str8_list);

      for (;;)
      {
        Json_token token = json_lexer_eat_next_token(lexer);
        if (token.kind == Json_token_kind__END)
        {
          is_early_exit = true;
          break;
        }
        else if (token.kind == Json_token_kind__double_quotes)
        { 
          break;
        }
        else
        {
          str8_list_push_str(scratch.arena, str_list, token.str);
        }
      }

      if (is_early_exit)
      {
        *parse_error_kind = Json_error_kind__unclosed_string;
        *parse_note = str8_from_list_v(arena, 8, 
                                       Str8FromClit("Error(Unclosed_string): \n"),
                                       Str8FromClit("  Unclosed string value, closing \" expected.")
        );
      }
      else
      {
        parse_value->u.str = str8_from_list(arena, str_list);
      }
    }
  }

  return parse_result;
}

Json_parse_result json_parse_array_value(Arena* arena, Json_lexer* lexer)
{
  Json_parse_result parse_result    = {};
  parse_result.value                = ArenaPush(arena, Json_value);
  parse_result.value->kind          = Json_value_kind__array; 
  
  // Aliases for easier usage 
  Json_value* parse_value           = parse_result.value;
  Json_error_kind* parse_error_kind = &parse_result.error;
  Str8* parse_note                  = &parse_result.note;

  Json_token left_brace_token = json_lexer_eat_next_token(lexer);
  if (left_brace_token.kind != Json_token_kind__left_square_brace)
  {
    *parse_error_kind = Json_error_kind__internal;
    *parse_note = str8_from_list_v(arena, 8, 
                                   Str8FromClit("Error(Internal): \n"),
                                   Str8FromClit(" -\""),
                                   Str8FromClit(__func__),
                                   Str8FromClit("\""),
                                   Str8FromClit(" was called by the parsing system, but the first token didnt match.\n"),
                                   Str8FromClit(" -Json_token_kind__left_square_brace was expected, but token kind "),
                                   str8_from_json_token_kind(left_brace_token.kind),
                                   Str8FromClit(" was matched insted.")
    );
  }
  else // This is the scope for success case
  {
    Json_array* arr = &parse_value->u.array;
    if (json_lexer_match_next_token(lexer, Json_token_kind__right_square_brace))
    {
      // Nothing here, we just got an empty array
    }
    else
    {
      for (B32 did_error_occur = false; !did_error_occur;) 
      { 
        Json_parse_result new_value_result = json_parse_value(arena, lexer);
        if (new_value_result.error) 
        {
          did_error_occur = true;
          *parse_error_kind = Json_error_kind__value_was_expected;
          *parse_note = str8_from_list_v(arena, 2,
                                         Str8FromClit(" -Value was expected. \n"),
                                         new_value_result.note
          ); 
        }
        else // Succ 
        {
          Json_value_node* new_node = ArenaPush(arena, Json_value_node);
          new_node->value = new_value_result.value;
          QueuePushBack(arr, new_node);
          arr->count += 1;

          // Check for another coma or a closing brace
          Json_token token_to_match = json_lexer_eat_next_token(lexer);
          if (token_to_match.kind == Json_token_kind__coma)
          {
            /* Keep looping over the values */ U32 value_for_debug_to_step_onto = 0; 
          }
          else if (token_to_match.kind == Json_token_kind__right_square_brace)
          {
            break; //Here we just end the array
          }
          else 
          {
            did_error_occur = true;
            *parse_error_kind = Json_error_kind__unexpected_token_insted_of_a_brace_or_coma_or_colon;
            *parse_note = str8_from_list_v(arena, 3, 
                                           Str8FromClit(" -Unexpected token: "),
                                           str8_from_json_token_kind(token_to_match.kind),
                                           Str8FromClit(", but a ',' or ']' was expected. ")
            );
          }
        }
      }
    }
  }
  return parse_result;
}

Json_parse_result json_parse_object_value(Arena* arena, Json_lexer* lexer)
{
  Json_parse_result parse_result = {};
  parse_result.value = ArenaPush(arena, Json_value);
  parse_result.value->kind = Json_value_kind__object;

  Json_value* parse_value      = parse_result.value;
  Json_error_kind* parse_error = &parse_result.error;
  Str8* parse_note             = &parse_result.note;
  
  Json_token left_brace_to_match = json_lexer_eat_next_token(lexer);
  if (left_brace_to_match.kind != Json_token_kind__left_curly_brace)
  {
    *parse_error = Json_error_kind__internal;
    *parse_note = str8_from_list_v(arena, 8, 
                                   Str8FromClit("Error(Internal): \n"),
                                   Str8FromClit(" -\""),
                                   Str8FromClit(__func__),
                                   Str8FromClit("\""),
                                   Str8FromClit(" was called by the parsing system, but the first token didnt match.\n"),
                                   Str8FromClit(" -Json_token_kind__left_curly_brace was expected, but token kind "),
                                   str8_from_json_token_kind(left_brace_to_match.kind),
                                   Str8FromClit(" was matched insted.")
    );
  }
  else // Success
  {
    Json_object* object = &parse_value->u.object;
    if (json_lexer_match_next_token(lexer, Json_token_kind__right_curly_brace))
    {
      // Nothing here, we just got an empty object
    }
    else
    {
      for (B32 did_error_occur = false; !did_error_occur;) 
      {
        // Getting the key
        Str8 new_pair_key = {};
        DefereInitReleaseLoop(Scratch scratch = get_scratch(&arena, 1), end_scratch(&scratch))
        {
          Json_parse_result key_result = json_parse_value(scratch.arena, lexer);
          if (key_result.error)
          {
            did_error_occur = true;
            *parse_error = Json_error_kind__object_pair_key_was_expected;
            *parse_note = str8_from_list_v(arena, 1,
                                           Str8FromClit(" -Object key was expected. \n ") 
            );
          }
          else if (key_result.value->kind != Json_value_kind__string)
          {
            did_error_occur = true;
            *parse_error = Json_error_kind__object_pair_key_was_expected;
            *parse_note = str8_from_list_v(arena, 1,
                                           Str8FromClit(" - Object key was expected, but a non string key was given. \n")
            );
          }
          else
          {
            new_pair_key = str8_from_str8_alloc(arena, key_result.value->u.str); 
          }
        }

        // Matching the ":"
        if (!did_error_occur)
        {
          Json_token colon_token_to_match = json_lexer_eat_next_token(lexer);
          if (colon_token_to_match.kind != Json_token_kind__colon)
          {
            did_error_occur = true;
            *parse_error = Json_error_kind__unexpected_token_insted_of_a_brace_or_coma_or_colon;
            *parse_note = str8_from_list_v(arena, 3,
                                           Str8FromClit(" - Colon separator for key value pair was expected, but "),
                                           colon_token_to_match.str,
                                           Str8FromClit(" was given. ")
            );
          }
        }

        // Getting the value
        Json_value* new_pair_value = 0;
        if (!did_error_occur)
        {
          Json_parse_result new_value_result = json_parse_value(arena, lexer);
          if (new_value_result.error)
          {
            did_error_occur = true;
            *parse_error = new_value_result.error;
            *parse_note = str8_from_list_v(arena, 2,
                                           Str8FromClit(" - Value for key value pair was expected. \n"),
                                           new_value_result.note
            );
          }
          else
          {
            new_pair_value = new_value_result.value;
          }
        }

        // Creating the new pair
        if (!did_error_occur)
        {
          Json_pair_node* new_pair_node = ArenaPush(arena, Json_pair_node);
          new_pair_node->pair.key = new_pair_key;
          new_pair_node->pair.value = new_pair_value;

          QueuePushBack(object, new_pair_node);
          object->count += 1;
        }

        // Checking if there is another element to be parsed or we done
        if (!did_error_occur)
        {
          Json_token token_to_match = json_lexer_eat_next_token(lexer);
          if (token_to_match.kind == Json_token_kind__coma)
          {
            /* Keep looping over the pairs */ U32 value_for_debug_to_step_onto = 0; 
          }
          else if (token_to_match.kind == Json_token_kind__right_curly_brace)
          {
            break; //Here we just end the object scope
          }
          else 
          {
            did_error_occur = true;
            *parse_error = Json_error_kind__unexpected_token_insted_of_a_brace_or_coma_or_colon;
            *parse_note = str8_from_list_v(arena, 2, 
                                           Str8FromClit(" -A coma or a curly brace was expected after "),
                                           Str8FromClit("a key value pair inside a json object.")
            ); 
          }
        }

      } // for (B32 did_error_occur = false; !did_error_occur;) 

    } // Succ
  }

  return parse_result;
}

Json_parse_result json_parse_value(Arena* arena, Json_lexer* lexer)
{
  // TODO:
  // Here we cant just = {}, cause the we are providing a null pointer to value and no error
  Json_parse_result parse_result = {};
  Json_value* parse_value        = parse_result.value;
  Json_error_kind* parse_error   = &parse_result.error;
  Str8* parse_note               = &parse_result.note;
  {
    Json_token peeked_token = json_lexer_peek_next_token(lexer);
    switch (peeked_token.kind)
    {
      default: { InvalidCodePath(); /* TODO: Internal error, this codepass shoud never be taken */ }
      case Json_token_kind__null:    
      case Json_token_kind__true:   
      case Json_token_kind__false:  
      { 
        json_lexer_eat_next_token(lexer); 
        if      (peeked_token.kind == Json_token_kind__null)   { parse_value->kind = Json_value_kind__null;   }
        else if (peeked_token.kind == Json_token_kind__true)   { parse_value->kind = Json_value_kind__true;   }
        else if (peeked_token.kind == Json_token_kind__false)  { parse_value->kind = Json_value_kind__false;  }
        else 
        {
          // TODO: handle error here (internal)
          Assert(false);
        }
      } break;

      case Json_token_kind__number:
      {
        parse_result = json_number_value_from_number_token(arena, lexer);
      } break;

      case Json_token_kind__double_quotes: 
      {
        parse_result = json_parse_string_value(arena, lexer);    
      } break;

      case Json_token_kind__left_square_brace:
      {
        parse_result = json_parse_array_value(arena, lexer);
      } break;

      case Json_token_kind__left_curly_brace:
      {
        parse_result = json_parse_object_value(arena, lexer);
      } break;

      // These are the once we dont expect to find here, so they all result in unexpected value errors
      // IDEA: There might be a reason to have these be more specific, but for now its fine
      case Json_token_kind__single_character: 
      case Json_token_kind__right_curly_brace: 
      case Json_token_kind__right_square_brace: 
      case Json_token_kind__colon: 
      case Json_token_kind__coma: 
      case Json_token_kind__END: 
      {
        *parse_error = Json_error_kind__unexpected_token_insted_of_a_value;
        *parse_note = str8_from_list_v(arena, 5,
                                       Str8FromClit(" -Unexpected token: "),
                                       Str8FromClit("\""),
                                       peeked_token.str,
                                       Str8FromClit("\""),
                                       Str8FromClit(", a value was expected. ")
        );
      } break;
    }
  }
  return parse_result;
}

#if 0

Json_parse_result test_create_ast_for_json(Arena* arena, Str8 json_as_str)
{
  Json_lexer lexer = {};
  lexer.input_text = json_as_str;

  Json_value* ast = 0; 
  if (!json_lexer_is_done(&lexer))
  {
    ast = json_parse_value(arena, &lexer);
    // Json is a single json value, so there shoud be nothing left after we get the value
    if (!json_lexer_is_done(&lexer))
    {
      // TODO: Some stuff is left there, report the error
      Assert(false);
    }
  }

  // TODO: I dont like the fact that it can be null, what happends when we dont have anything inside json?
  return ast;
}

///////////////////////////////////////////////////////////
// Damian: Other
//
void _json_debug_recreate_json_HELPER(Arena* arena, Json_value* ast, Str8_list* out_json_list)
{
  switch (ast->kind)
  {
    default: { Assert(false); } break;
    
    case Json_value_kind__null: 
    {
      str8_list_push_str(arena, out_json_list, Str8FromClit("null"));      
    } break;
    
    case Json_value_kind__true:
    {
      str8_list_push_str(arena, out_json_list, Str8FromClit("true"));      
    } break;

    case Json_value_kind__false:
    {
      str8_list_push_str(arena, out_json_list, Str8FromClit("false"));      
    } break;

    case Json_value_kind__number:
    {
      S64 n = ast->u.number;
      str8_list_push_str(arena, out_json_list, str8_from_fmt_alloc(arena, "#S64", n));      
    } break;

    case Json_value_kind__string:
    {
      Str8 str = ast->u.str;
      str8_list_push_str(arena, out_json_list, Str8FromClit("\""));      
      str8_list_push_str(arena, out_json_list, str8_from_str8_alloc(arena, str));      
      str8_list_push_str(arena, out_json_list, Str8FromClit("\""));      
    } break;

    case Json_value_kind__array:
    {
      Json_array* arr = &ast->u.array;
      // [1, 2, 3, 4]
      str8_list_push_str(arena, out_json_list, Str8FromClit("["));
      U64 child_index = 0;
      for (Json_value_node* node = arr->first; node != 0; node = node->next, child_index += 1)
      {
        Json_value* nested_value = node->value;
        _json_debug_recreate_json_HELPER(arena, nested_value, out_json_list);
        if (arr->count > 1 && child_index != (arr->count - 1))
        {
          str8_list_push_str(arena, out_json_list, Str8FromClit(", "));
        }
      }
      str8_list_push_str(arena, out_json_list, Str8FromClit("]"));

    } break;

    case Json_value_kind__object:
    {
      Json_object* obj = &ast->u.object;
      
      str8_list_push_str(arena, out_json_list, Str8FromClit("{"));
      U64 node_index = 0;
      for (Json_pair_node* node = obj->first; node != 0; node = node->next, node_index += 1)
      {
        str8_list_push_str(arena, out_json_list, str8_from_fmt_alloc(arena, "\"#Str8\": ", node->pair.key));
        _json_debug_recreate_json_HELPER(arena, node->pair.value, out_json_list);
        if (obj->count > 1 && node_index != (obj->count - 1))
        {
          str8_list_push_str(arena, out_json_list, Str8FromClit(", "));
        }
      }
      str8_list_push_str(arena, out_json_list, Str8FromClit("}"));
    } break;
  }
}

Str8 json_debug_recreate_json(Arena* arena, Json_value* ast)
{
  Str8 str = {};
  DefereInitReleaseLoop(Scratch scratch = get_scratch(&arena, 1), end_scratch(&scratch))
  {
    Str8_list* json_list = ArenaPush(scratch.arena, Str8_list);
    _json_debug_recreate_json_HELPER(scratch.arena, ast, json_list);
    str = str8_from_list(arena, json_list);
  }
  return str;

}

#endif

void json_debug_print(Json_value* ast)
{
  switch (ast->kind)
  {
    case Json_value_kind__NULL: { Assert(false); } break;

    case Json_value_kind__null: { str8_printf("#Str8", Str8FromClit("null")); } break;
    case Json_value_kind__true: { str8_printf("#Str8", Str8FromClit("true")); } break;
    case Json_value_kind__false: { str8_printf("#Str8", Str8FromClit("false")); } break;
    case Json_value_kind__number: { str8_printf("#S64", ast->u.number); } break; 
    case Json_value_kind__string: { str8_printf("#Str8", ast->u.str); } break; 
    
    case Json_value_kind__array: 
    {
      Json_array* arr = &ast->u.array;
      str8_printf("[");
      for (Json_value_node* node = arr->first; node != 0; node = node->next)
      {
        str8_printf(" ");
        json_debug_print(node->value);
        str8_printf(" ,");
      }
      str8_printf("]");
    }
    break;

    case Json_value_kind__object: 
    {
      Json_object* obj = &ast->u.object;
      str8_printf("{");
      for (Json_pair_node* node = obj->first; node != 0; node = node->next)
      {
        str8_printf(" #Str8", node->pair.key);
        str8_printf(" : ");
        json_debug_print(node->pair.value);
        str8_printf(",");
      }
      str8_printf("}");
    }
    break;
  }
}

#endif