#ifndef D_JSON_H
#define D_JSON_H

#include "base/include.h"
#include "base/include.cpp" // TODO: Remove this later

/* ECMA json format 
Tokens: { } [ ] : , true false null
Values: json object, array, number, string, true, false, null

Plan:
There is a limited number of things that go into tokens, this means thast i can just create all them.
Problem there might be with numbers, since numbers are not sinmple tokens, but this might also be fine if we just 
create a couple of number types just at lex time.

Do we want to create all the tokens up front or ask for them when parsing.
Asking for them mid parsing is nice cause then we dont have to allocate anything.
Creating them up front doesnt change much at all, but also gives us an ability to peek in as far as we need. 
Do we need to peek tho? Well not really i guess, cause all the errors might still be found just by asking for the tokens either way.
Do we gain anything in terams of performace by pre tokenizing stuff?

1) Token creation up front
2) Paring -> Ast + parsing errors
3) API to get stuff from the json
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

B32 json_lexer_is_done(Json_lexer* lexer)
{
  B32 is_done = (lexer->current_pos == lexer->input_text.count);
  return is_done;
}

Json_token json_lexer_make_token(Json_lexer* lexer, Json_token_kind token_kind)
{
  Json_token token = {};
  token.kind = token_kind;
  token.str = str8_substring_index(lexer->input_text, lexer->token_start_pos, lexer->current_pos);
  return token;
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
  return is_match;
}

// TODO: Add comments support

Json_token json_lexer_eat_next_token(Json_lexer* lexer)
{
  Json_token result_token = {};
  if (json_lexer_is_done(lexer))
  {
    result_token = json_lexer_make_token(lexer, Json_token_kind__END);
  }
  else
  {
    for (;;)
    {
      char ch = json_lexer_peek_char(lexer);
      if (ch == ' ' || ch == '\n' || ch == '\r')
      {
        json_lexer_eat_char(lexer);
      }
      else
      {
        break;
      }
    }
    
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

        // Damian: I a json there cant be a + sign before a number, nor can there be a leading 0, unless the value is 0
        //         I might allow the + sign to be there, cause just why not, and also the leading 0, cause why not
        // 43
        // -34
        // +43
        // +0034
        // -0034
        if (is_char_a_number(first_char) || first_char == '-' || first_char == '+')
        {
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


// ---

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
  Json_error_kind__NONE,
  Json_error_kind__internal,
  Json_error_kind__unclosed_string,
};

// Predecl
Json_value* json_parse_value(Arena* arena, Json_lexer* lexer);

Json_value* json_number_value_from_number_token(Arena* arena, Json_lexer* lexer)
{
  Json_value* result_value = ArenaPush(arena, Json_value);
  result_value->kind = Json_value_kind__number;

  Json_token number_token = json_lexer_eat_next_token(lexer);
  if (number_token.kind != Json_token_kind__number)
  {
    Assert(false);
    // TODO: Error internal
  }
  else
  {
    S64 value = s64_from_str8(number_token.str);
    result_value->u.number = value;
  }

  return result_value;
} 

Json_value* json_parse_string_value(Arena* arena, Json_lexer* lexer, Json_error_kind* out_error_kind)
{
  B32 matched_start = json_lexer_match_next_token(lexer, Json_token_kind__double_quotes);
  Assert(matched_start); 
  
  Json_value* result_value = ArenaPush(arena, Json_value);
  result_value->kind = Json_value_kind__string;
  if (out_error_kind) { *out_error_kind = Json_error_kind__NONE; }

  if (!matched_start)
  {
    if (out_error_kind) { // TODO: This has to be then removed if Result type is introduced
      *out_error_kind = Json_error_kind__internal;
    }
  }
  else
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
        if (out_error_kind) {
          *out_error_kind = Json_error_kind__unclosed_string;
        }
      }
      else
      {
        result_value->u.str = str8_from_list(arena, str_list);
      }
    }
  }

  return result_value;
}

Json_value* json_parse_array_value(Arena* arena, Json_lexer* lexer, Json_error_kind* out_error_kind)
{
  B32 is_matched = json_lexer_match_next_token(lexer, Json_token_kind__left_square_brace);
  
  Json_value* result_arr_value = ArenaPush(arena, Json_value);
  result_arr_value->kind = Json_value_kind__array;
  Json_array* arr = &result_arr_value->u.array;
  
  if (!is_matched)
  {
    Assert(is_matched);
    if (out_error_kind) { *out_error_kind = Json_error_kind__internal; }
  }
  else
  {
    if (json_lexer_match_next_token(lexer, Json_token_kind__right_square_brace))
    {
      // Nothing here, we are dont with the array, it has 0 elements
    }
    else
    {
      for (;;)
      {
        // TODO: See what happend here for end value
        Json_value* new_value = json_parse_value(arena, lexer); // We know if there is an error here, cause the recursive call will shows us 
        Json_value_node* new_node = ArenaPush(arena, Json_value_node);
        new_node->value = new_value;
        QueuePushBack(arr, new_node);
        arr->count += 1;
        
        // Damian: Normal json doesnt always expect another value after a coma, i dont like that,
        //         i want to have it be symetrical like enums, so am adding this feature

        json_lexer_match_next_token(lexer, Json_token_kind__coma); // Come is optional there, so we match
        if (json_lexer_match_next_token(lexer, Json_token_kind__right_square_brace))
        {
          break; 
        } 
        else { /* Just some other token after the coma that is not a brace, will be handled next for loop iter */ }
      }
    }
  }
  return result_arr_value;
}

Json_value* json_parse_object_value(Arena* arena, Json_lexer* lexer, Json_error_kind* out_error_kind)
{
  B32 is_matched = json_lexer_match_next_token(lexer, Json_token_kind__left_curly_brace);
  
  Json_value* result_object_value = ArenaPush(arena, Json_value);
  result_object_value->kind = Json_value_kind__object;
  Json_object* object = &result_object_value->u.object;

  if (!is_matched)
  {
    Assert(is_matched);
    if (out_error_kind) { *out_error_kind = Json_error_kind__internal; }
  }
  else
  {
    if (json_lexer_match_next_token(lexer, Json_token_kind__right_curly_brace))
    {
      // We just got an empty json onject: {}
    }
    else
    {
      for (;;)
      {
        // obj --> pair, pair ...
        // pair -> key, value
        // obj --> (str, value), (str, value), ...
        Json_pair_node* new_node = ArenaPush(arena, Json_pair_node);
        {
          // TODO: Maybe dont use scratch here, i kind dont like it here to be honest.
          //       This scratch makes me copy every key every time, this is systematic thing now,
          //       have to do somthing different here then
          B32 is_key_a_string = false;
          Str8 str_key = {};
          DefereInitReleaseLoop(Scratch scratch = get_scratch(&arena, 1), end_scratch(&scratch))
          {
            Json_value* key_value = json_parse_value(scratch.arena, lexer);
            if (key_value->kind == Json_value_kind__string)
            {
              is_key_a_string = true;
              str_key = str8_from_str8_alloc(arena, key_value->u.str);
            }
          }
          if (!is_key_a_string)
          {
            // TODO: This is a json error: we just got a non string key
            Assert(false);
          }
          else
          {
            B32 is_colon_matched = json_lexer_match_next_token(lexer, Json_token_kind__colon);
            if (!is_colon_matched)
            {
              // TODO: Json error: no colon after a string key for a json object pair
              Assert(false);
            } 
            else
            {
              Json_value* value = json_parse_value(arena, lexer);
              new_node->pair.value = value;
              new_node->pair.key = str_key;

              QueuePushBack(object, new_node);
              object->count += 1;
              
              json_lexer_match_next_token(lexer, Json_token_kind__coma); // Come is optional there, so we match
              if (json_lexer_match_next_token(lexer, Json_token_kind__right_curly_brace))
              {
                break; 
              } 
              else { /* Just some other token after the coma that is not a brace, will be handled next for loop iter */ }
            }
          }
        }
      }
    }
  }
  return result_object_value;
}

Json_value* json_parse_value(Arena* arena, Json_lexer* lexer)
{
  Json_value* value = 0;
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
        value = ArenaPush(arena, Json_value);
        if      (peeked_token.kind == Json_token_kind__null)   { value->kind = Json_value_kind__null;   }
        else if (peeked_token.kind == Json_token_kind__true)   { value->kind = Json_value_kind__true;   }
        else if (peeked_token.kind == Json_token_kind__false)  { value->kind = Json_value_kind__false;  }
        else 
        {
          // TODO: handle error here (internal)
          Assert(false);
        }
      } break;

      case Json_token_kind__number:
      {
        value = json_number_value_from_number_token(arena, lexer);
      } break;

      case Json_token_kind__double_quotes: 
      {
        value = json_parse_string_value(arena, lexer, 0);    
      } break;

      case Json_token_kind__left_square_brace:
      {
        value = json_parse_array_value(arena, lexer, 0);
      } break;

      case Json_token_kind__left_curly_brace:
      {
        value = json_parse_object_value(arena, lexer, 0);
      }
    }
  }
  return value;
}

Json_value* test_create_ast_for_json(Arena* arena, Str8 json_as_str)
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

// ------

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



// Insignificant whitespace is allowed before or after any token. Whitespace is any sequence of one or more of
// the following code points: character tabulation (U+0009), line feed (U+000A), carriage return (U+000D), and
// space (U+0020). Whitespace is not allowed within any token, except that space is allowed in strings.

// Json can be any combination of its value, even just a number value is a valid json --> 42
// Another example --> { x: 42 } 
// Another example --> [42] --> [42, 43]
// Another example --> {}
// Another example --> "string"





























#endif











