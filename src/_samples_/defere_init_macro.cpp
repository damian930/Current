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
  DefereInitReleaseLoop(Scratch scratch1 = get_scratch(), end_scratch(&scratch1))
  {
    Str8 str1 = Str8FromClit(scratch1.arena, "1");
    printf("%s \n", str1.data);

    DefereInitReleaseLoop(Scratch scratch2 = get_scratch(), end_scratch(&scratch2))
    {
      Str8 str2 = Str8FromClit(scratch2.arena, "2");
      printf("%s \n", str2.data);
    }
  
    DefereInitReleaseLoop(Scratch scratch3 = get_scratch(), end_scratch(&scratch3))
    {
      Str8 str3 = Str8FromClit(scratch3.arena, "3");
      printf("%s \n", str3.data);
    
      DefereInitReleaseLoop(Scratch scratch3 = get_scratch(), end_scratch(&scratch3))
      {
        str1 = Str8FromClit(scratch3.arena, "3");
        printf("%s \n", str1.data);
      }
    }

    Str8 str11 = Str8FromClit(scratch1.arena, "1");
    printf("%s \n", str11.data);
  }


  DefereInitReleaseLoop(Scratch scratch = get_scratch(), end_scratch(&scratch))
  {
    
  }

}








