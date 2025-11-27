#ifndef D_PROCESS_TRACKER_CPP
#define D_PROCESS_TRACKER_CPP

#include "process_tracker.h"

#if 1
Win32_snapshot_process_list* win32_get_snapshot_and_process_handles(Arena* arena)
{
  Win32_snapshot_process_list* result_list = ArenaPush(arena, Win32_snapshot_process_list);
  
  HANDLE snapshot_handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, Null);
  result_list->snapshot_handle = snapshot_handle;  
  AssertTodo(snapshot_handle != INVALID_HANDLE_VALUE);

  // Get all the handles for all the proccesss here
  {
    PROCESSENTRY32 process_entry = {};
    process_entry.dwSize = sizeof(PROCESSENTRY32);
    
    { // Getting the first one 
      BOOL succ = Process32First(snapshot_handle, &process_entry);  
      AssertTodo(succ);  
    }
    
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
    // This also return win32 error code 18 when return fails after the last process and exits the while loop
  }

  return result_list;
}

Process_data_list* get_all_process_data(Arena* arena)
{
  Process_data_list* result_list = ArenaPush(arena, Process_data_list);
  {
    Win32_snapshot_process_list* win32_process_handle_list = win32_get_snapshot_and_process_handles(arena);

    for (Win32_process_node* win32_node = win32_process_handle_list->first; 
       win32_node != 0; 
       win32_node = win32_node->next
    ) {
      HANDLE handle = win32_node->process_handle;

      // Creating a new data entry for a process
      Process_data* process_data = 0;
      {
        Process_data_node* new_node = ArenaPush(arena, Process_data_node);
        // QueuePushBack(result_list, new_node);
        DllPushBack(result_list, new_node);
        result_list->count += 1;
        process_data = &new_node->process_data;
      }

      // Getting the process file path
      {
        // TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:
        // TODO: Think about this here, scratch on scratch issue :TODO:
        // TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:TODO:
        {
          Scratch scratch = get_scratch(&arena, 1); 
          Data_buffer name_buffer = data_buffer_make(scratch.arena, 512);
          U32 bytes_written_no_nt = (U32)name_buffer.count;  
          BOOL succ = QueryFullProcessImageNameA(handle, 0, (CHAR*)name_buffer.data, (DWORD*)&bytes_written_no_nt); 
          end_scratch(&scratch);
          if (succ) 
          {
            Str8 name = str8_substring_range(name_buffer, range_u64(0, bytes_written_no_nt));
            process_data->path = str8_from_str8_alloc(arena, name);
          }
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

        process_data->creation_time = time_from_win32_system_time(&s_creation_time) ;
      }
    }
  }

  return result_list;
}
#endif







#endif