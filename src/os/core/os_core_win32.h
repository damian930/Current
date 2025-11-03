#ifndef WIN32_CORE_H
#define WIN32_CORE_H

#include <windows.h>

#include "base/core.h"
#include "base/string.h"
#include "base/arena.h"

// TODO: Mem commit / release / free / protect

enum File_access_flags : U32 {
  File_access_flag_read        = (1 << 0),
  File_access_flag_write       = (1 << 1),
  File_access_flag_execute     = (1 << 2),
  File_access_flag_append      = (1 << 3),
  File_access_flag_share_read  = (1 << 4),
  File_access_flag_share_write = (1 << 5),
};

struct Win32_error {
  Win32_error* prev;
  Win32_error* next;
  Str8 note;
};

struct Win32_error_stack {
  Win32_error* first;
  Win32_error* last;
  U64 error_count;
};

struct Win32_file {
  HANDLE handle;
};

// NOTE: Static data for win32 
struct DEBUG_win32_state {
  F64 performance_freq_per_sec;
};

// TODO: Move this to a better place
global DEBUG_win32_state g_win32_state = {}; 
void DEBUG_win32_init()
{
  // Performace freq
  {
    LARGE_INTEGER lr = {};
    QueryPerformanceFrequency(&lr);
    g_win32_state.performance_freq_per_sec = (F64)lr.QuadPart;
  }
}

void DEBUG_win32_end()
{
  // Nothing here for
}

F64 get_monotonic_time()
{
  // TODO: see if testing for the win32 state init needed here

  F64 perf_counter = 0;
  {
    LARGE_INTEGER lr = {};
    QueryPerformanceCounter(&lr);
    perf_counter = (F64)lr.QuadPart;
  }

  F64 time_in_sec = perf_counter / g_win32_state.performance_freq_per_sec;
  return time_in_sec;
}

// TODO: If this is usefull and tested, then make this more formal
void win32_fata_error(const char* message)
{
  // MessageBoxA(Null, message, "Error", MB_ICONEXCLAMATION);
  ExitProcess(0);
}

// TODO: Add error hadeling here
Win32_file open_file(Str8 file_path, U32 access_flags)
{
  Scratch scratch = get_scratch();

  DWORD desired_access = 0;
  if (access_flags & File_access_flag_read) { desired_access |= GENERIC_READ; }
  if (access_flags & File_access_flag_write) { desired_access |= GENERIC_WRITE; }
  if (access_flags & File_access_flag_execute) { desired_access |= GENERIC_EXECUTE; }
  if (access_flags & File_access_flag_append) { desired_access |= GENERIC_WRITE; }
  
  DWORD share_mode = 0;
  if (access_flags & File_access_flag_share_read) { share_mode |= FILE_SHARE_READ; }
  if (access_flags & File_access_flag_share_write) { share_mode |= FILE_SHARE_WRITE; } 

  DWORD creation_parameters = 0;
  if (access_flags & File_access_flag_append) { creation_parameters = OPEN_ALWAYS; }

  if (access_flags & File_access_flag_read) { creation_parameters = OPEN_EXISTING; }
  if (access_flags & File_access_flag_write) { creation_parameters = CREATE_NEW; } // Truncated if exists
  if (access_flags & File_access_flag_append) { creation_parameters = OPEN_ALWAYS; } // Doesnt truncate

  Str8 file_path_null_term = str8_from_str8_temp_null_term(scratch.arena, file_path);

  HANDLE file_handle = CreateFileA((CHAR *)file_path_null_term.data, 
                                   desired_access, 
                                   share_mode,
                                   Null,
                                   creation_parameters,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);
  // TODO: Do something when it fails
  Assert(file_handle != INVALID_HANDLE_VALUE);
  
  end_scratch(&scratch);

  Win32_file file = {};
  file.handle = file_handle; 

  return file;
}

void close_file(Win32_file file)
{
  CloseHandle(file.handle);
  file.handle = INVALID_HANDLE_VALUE;
}

Data_buffer read_file(Arena* arena, Win32_file file)
{
  SetFilePointerEx(file.handle, LARGE_INTEGER{}, Null, FILE_BEGIN);

  U64 file_size = {};
  {
    LARGE_INTEGER lr = {};
    GetFileSizeEx(file.handle, &lr);
    file_size = lr.QuadPart;
  }

  Data_buffer buffer = {};
  buffer.count = file_size;
  buffer.data = ArenaPushArr(arena, U8, file_size);

  // TODO: see if dword is the same as u32
  U32 bytes_read = 0;
  ReadFile(file.handle, buffer.data, file_size, (unsigned long*)&bytes_read, Null);

  return buffer;
}

void write_to_file(Win32_file file, Data_buffer buffer)
{
  SetFilePointerEx(file.handle, LARGE_INTEGER{}, Null, FILE_BEGIN);

  U32 bytes_writen = 0;
  WriteFile(file.handle, buffer.data, buffer.count, (unsigned long*)&bytes_writen, NULL);
}



#endif
















