
// #include "base/string.h"
// #include "base/string.cpp"

#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp"

#include <stdio.h>

#include <tlhelp32.h>	
#include <psapi.h>
#pragma comment(lib, "Kernel32.lib")

void test_getting_data_from_processes()
{
  HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, Null);
  Assert(snapshot_handle != INVALID_HANDLE_VALUE); 

  // Get all the handles for all the proccesss here
  {
    PROCESSENTRY32 process_entry = {};
    Process32First(snapshot_handle, &process_entry); // TODO: Might fail 
    process_entry.dwSize = sizeof(PROCESSENTRY32);
    
    U32 counter = 0;
    do 
    {
      DefereLoop(printf("--%d-----------------------------\n", ++counter), printf("\n"))
      {

        HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_entry.th32ProcessID);
        if (process_handle)
        {
          // Getting the process file path
          {
            Str8 name = {};
            Scratch scratch = get_scratch();
            {
              Data_buffer name_buffer = data_buffer_make(scratch.arena, 512);
              U32 bytes_written_no_nt = name_buffer.count;  
              BOOL succ = QueryFullProcessImageNameA(process_handle, 0, (CHAR*)name_buffer.data, (DWORD*)&bytes_written_no_nt); 
              if (succ) 
              {
                name = str8_substring_range(name_buffer, range_u64(0, bytes_written_no_nt));
              }
            }
            printf("Name: %s \n", str8_temp_from_str8(name).data);
            end_scratch(&scratch);
          }
          
          // Getting process times
          {
            FILETIME f_creation_time = {}; 
            FILETIME f_exit_time     = {};
            FILETIME f_kernel_time   = {};
            FILETIME f_user_time     = {};
            BOOL succ = GetProcessTimes(process_handle, &f_creation_time, &f_exit_time, &f_kernel_time, &f_user_time);
            Assert(succ);
            
            SYSTEMTIME s_creation_time = {}; 
            SYSTEMTIME s_exit_time     = {};
            SYSTEMTIME s_kernel_time   = {};
            SYSTEMTIME s_user_time     = {};
            BOOL succ1 = FileTimeToSystemTime(&f_creation_time, &s_creation_time);
            BOOL succ2 = FileTimeToSystemTime(&f_exit_time, &s_exit_time);
            BOOL succ3 = FileTimeToSystemTime(&f_kernel_time, &s_kernel_time);
            BOOL succ4 = FileTimeToSystemTime(&f_user_time, &s_user_time);
            Assert(succ1);
            Assert(succ2);
            Assert(succ3);
            Assert(succ4);
          }
        }
        else {
          CloseHandle(process_handle);
        }
      }
    }
    while (Process32Next(snapshot_handle, &process_entry)); 
    // This also return win32 error code 18 when returns fals and exits the while loop
  }
    
  CloseHandle(snapshot_handle);
}


///////////////////////////////////////////////////////////
// Damian: Main entry point
//
void EntryPoint();

int main()
{
  DefereLoop(os_win32_state_init(), os_win32_state_release())
  {
    EntryPoint();
  }

  return 0;
} 

/* Features:
  [] - Active process
  [] - Process name 
  [] - Process start time 
  [] - Process active duration time 
  [] - Process icon 
  [] - Process filtering
       [] - Apps (Telegram, HS, Fortnite ...)  
  [] - 
  [] - 
  [] - 
  [] - 
  [] - 
*/

void EntryPoint()
{
  Arena* arena = arena_alloc(Kilobytes_U64(100), "Process sample arena");
  printf("--- Getting data : Start --- \n");
  test_getting_data_from_processes();
  printf("--- Getting data : End --- \n");

}

#if 0
///////////////////////////////////////////////////////////
// Damian: Sample code
//
struct Win32_process_node {
  Win32_process_node* next;
  HANDLE process_handle;
};

struct Win32_snapshot_process_list {
  HANDLE snapshot_handle;
  Win32_process_node* first;
  Win32_process_node* last;
  U32 count;
  U32 skip_count;
};

Win32_snapshot_process_list* test_prepare_win32_process_handles(Arena* arena)
{
  Win32_snapshot_process_list* result_list = ArenaPush(arena, Win32_snapshot_process_list);
  
  HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, Null);
  result_list->snapshot_handle = snapshot_handle;  
  Assert(snapshot_handle != INVALID_HANDLE_VALUE); //TODO: Might be invalid handle here
  

  // Get all the handles for all the proccesss here
  {
    PROCESSENTRY32 process_entry = {};
    Process32First(snapshot_handle, &process_entry); // TODO: Might fail 
    process_entry.dwSize = sizeof(PROCESSENTRY32);
    
    do 
    {
      HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, process_entry.th32ProcessID);
      if (process_handle)
      {
        Win32_process_node* node = ArenaPush(arena, Win32_process_node);
        node->process_handle = process_handle;
        QueuePushBack(result_list, node);
        result_list->count += 1;
      }
      else {
        CloseHandle(process_handle);
      }
    }
    while (Process32Next(snapshot_handle, &process_entry)); 
    // This also return win32 error code 18 when returns fals and exits the while loop
  }

  return result_list;
}

struct Process_data {
  Str8 path;
  SYSTEMTIME creation_time;
};

struct Process_data_node {
  Process_data_node* next;
  Process_data process_data;
};

struct Process_data_list {
  Process_data_node* first;
  Process_data_node* last;
  U32 count;
};

Process_data_list* test_get_all_process_data(Arena* arena)
{
  Win32_snapshot_process_list* win32_process_handle_list = test_prepare_win32_process_handles(arena);

  Process_data_list* result_list = ArenaPush(arena, Process_data_list);

  Scratch scratch = get_scratch();
  for (Win32_process_node* win32_node = win32_process_handle_list->first; 
       win32_node != 0; 
       win32_node = win32_node->next
  ) {
    HANDLE handle = win32_node->process_handle;

    // Storing all the data we got
    Process_data* process_data = 0;
    {
      Process_data_node* new_node = ArenaPush(arena, Process_data_node);
      QueuePushBack(result_list, new_node);
      result_list->count += 1;
      process_data = &new_node->process_data;
    }

    // Getting the process file path
    {
      Data_buffer name_buffer = data_buffer_make(scratch.arena, 512);
      U32 bytes_written_no_nt = name_buffer.count;  
      BOOL succ = QueryFullProcessImageNameA(handle, 0, (CHAR*)name_buffer.data, (DWORD*)&bytes_written_no_nt); 
      if (succ) 
      {
        Str8 name = str8_substring(name_buffer, 0, bytes_written_no_nt);
        process_data->path = str8_from_str8(arena, name);
      }
    }
    
    // Getting process times
    {
      FILETIME f_creation_time = {}; 
      FILETIME f_exit_time     = {};
      FILETIME f_kernel_time   = {};
      FILETIME f_user_time     = {};
      BOOL succ = GetProcessTimes(handle, &f_creation_time, &f_exit_time, &f_kernel_time, &f_user_time);
      Assert(succ);

      SYSTEMTIME s_creation_time = {}; 
      SYSTEMTIME s_exit_time     = {};
      SYSTEMTIME s_kernel_time   = {};
      SYSTEMTIME s_user_time     = {};
      BOOL succ1 = FileTimeToSystemTime(&f_creation_time, &s_creation_time);
      BOOL succ2 = FileTimeToSystemTime(&f_exit_time, &s_exit_time);
      BOOL succ3 = FileTimeToSystemTime(&f_kernel_time, &s_kernel_time);
      BOOL succ4 = FileTimeToSystemTime(&f_user_time, &s_user_time);
      Assert(succ1);
      Assert(succ2);
      Assert(succ3);
      Assert(succ4);

      process_data->creation_time = s_creation_time;
    }
    
  }
  end_scratch(&scratch);

  return result_list;
}

///////////////////////////////////////////////////////////
// Damian: Main entry point
//
void EntryPoint();

int main()
{
  DefereLoop(os_win32_state_init(), os_win32_state_release())
  {
    EntryPoint();
  }

  return 0;
} 

/* Features:
  [] - Active process
  [] - Process name 
  [] - Process start time 
  [] - Process active duration time 
  [] - Process icon 
  [] - Process filtering
       [] - Apps (Telegram, HS, Fortnite ...)  
  [] - 
  [] - 
  [] - 
  [] - 
  [] - 
*/

void EntryPoint()
{
  Arena* arena = arena_alloc(Kilobytes_U64(100), "Process sample arena");
  
  Process_data_list* data_list = test_get_all_process_data(arena);
  for (Process_data_node* node = data_list->first; node != 0; node = node->next)
  {
    Process_data* data = &node->process_data;
    printf("----------------------------- \n");
    printf("Path: %s \n", str8_temp_from_str8(data->path).data);
    printf("Time: h:%d, m:%d \n" , data->creation_time.wHour, data->creation_time.wMinute);
    printf("----------------------------- \n");

  }

  // // TODO: Clear os resourses for the handles and the snapshot + the arena  

}


#endif