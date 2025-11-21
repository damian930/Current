#include "base/include.h"
#include "base/include.cpp"

#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp"

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
  Arena* arena = arena_alloc(Kilobytes_U64(64), "Arena just main");
  
  Str8_fmt_lexer lexer = {};
  lexer.input_str = Str8FromClit("--> #U32:<10 <-- ");

  while (str8_fmt_lexer_is_alive(&lexer))
  {
    Str8_fmt_token token = str8_fmt_lexer_eat_next_token(&lexer);
    str8_printf("Token_str: #Str8 \n", token.str);
  }

  // for (U32 i = 10; i < 10000000; i *= 10)
  // {
  //   // str8_printf("I --> #U32 <-- \n", i);
  //   str8_printf("I --> #U32:<10 <-- \n", i);
  // }



  // TODO:
  // {value:>10} - right align in width 10
  // {value:<10} - left align in width 10
  // {value:^10} - center align in width 10

}









