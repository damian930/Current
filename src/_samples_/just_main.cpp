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
  Str8 str = str8_from_fmt_alloc(arena, "X: #U32 \n", 5);
  str8_printf("X: #U32 \n", 5);
  // printf("%s \n", str.data);
}









