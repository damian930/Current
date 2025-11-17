#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp"

#include "base/string.h"
#include "base/string.cpp"

#include "base/arena.h"
#include "base/arena.cpp"

#pragma comment(lib, "User32.lib")

void EntryPoint();

struct Node {
  Node* next;
  Node* prev;
  Str8 str;
};

struct List {
  Node* first;
  Node* last;
};

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
  Arena* test_arena = arena_alloc(Gigabytes_U64(8), "test");
  OS_Win32_file file = os_win32_file_open(str8_temp_from_cstr("5GB file"), File_access_flag_read);
  OS_Win32_file_data data = os_win32_file_data(file);
  Data_buffer buffer = os_win32_file_read(test_arena, file);
  Assert(buffer.count == data.size);
  os_win32_file_close(file);

  os_win32_exit(1);

  // Testing the temp strings that augment the scratch allocation so i dont have to get scratched every time
  // Str8 str1 = str8_temp_from_cstr("Flopper");
  // printf("%s \n", str1.data);
  // printf("----\n");
  // Str8 str2 = str8_temp_from_cstr("2");
  // printf("%s \n", str1.data);
  // printf("%s \n", str2.data);
  // printf("----\n");
  // Str8 str3 = str8_temp_from_cstr("3");
  // printf("%s \n", str1.data);
  // printf("%s \n", str2.data);
  // printf("%s \n", str3.data);

  // os_win32_exit(1);

  // // Testing the first parameter for VirtualAlloc
  // {
  //   U64 size_to_reserve_rounded = align_up(Kilobytes_U64(4), g_os_win32_state.page_size); 
    
  //   U8* mem1 = (U8*)VirtualAlloc((void*)(2 * Kilobytes_U64(64)), size_to_reserve_rounded, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  //   printf("Mem: %lld \n", (U64)mem1);

  //   U8* mem2 = (U8*)VirtualAlloc((void*)(1 * Kilobytes_U64(64)), size_to_reserve_rounded, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
  //   printf("Mem: %lld \n", (U64)mem2);

  //   // U8* mem3 = (U8*)VirtualAlloc((void*)(2 * Kilobytes_U64(64)), size_to_reserve_rounded, MEM_RESERVE|MEM_COMMIT, PAGE_NOACCESS);
  //   // printf("Mem: %lld \n", (U64)mem3);
    
  //   *mem2 = 5;
  //   printf("Mem1 = %d \n", *mem2);
  //   printf("Mem2 = %d \n", *mem2);
  //   // printf("Mem3 = %d \n", *mem3);
  // }

  Arena* file_arena = arena_alloc(Gigabytes_U64(7), "File arena");
  {
    OS_Win32_file file = os_win32_file_open(Str8FromClit(file_arena, "5GB file"), File_access_flag_write);
    {
      // Create new one if doesnt exist
      // 

      Data_buffer data = data_buffer_make(file_arena, Gigabytes_U64(6));
      // ForEachEx(i, data.count, data.data)
      // {
        // data.data[i] = 5; 
      // }
      os_win32_file_write(file, data);
      // test_file_attrs(file);
    }
    os_win32_file_close(file);
    
  }
  arena_release(file_arena);

  // {
  //   Data_buffer file_data = os_win32_read_file_inplace(file_arena, Str8FromClit(file_arena, "../main.cpp"));
  //   ForEachEx(i, file_data.count, file_data) {
  //     printf("%c", file_data.data[i]);
  //   }
  //   arena_release(file_arena);
  // }
  
  // os_win32_display_fatal_error("Memory error", "Ran out of memory");
  // os_win32_exit(1);

}


