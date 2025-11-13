#ifndef WIN32_CORE_CPP
#define WIN32_CORE_CPP

#include "os_core_win32.h"
#include "base/math.cpp"

// TODO: Make sure inside the funtion that use os that os has been inited
//       This is particulary important here, since we are not using dyn pointers for state data
//       that get nulled when state is released.

///////////////////////////////////////////////////////////
// Damian: State
//
void os_win32_init()
{
  // Performace freq
  {
    LARGE_INTEGER lr = {};
    QueryPerformanceFrequency(&lr);
    g_os_win32_state.performance_freq_per_sec = (F64)lr.QuadPart;
  }

  // Sys info
  {
    SYSTEM_INFO info = {};
    GetSystemInfo(&info);
    g_os_win32_state.page_size = (U32)info.dwPageSize;
    g_os_win32_state.allocation_granularity = (U32)info.dwAllocationGranularity;
  }

  scratch_arenas[0] = arena_alloc(Gigabytes_U64(1), "scratch arena 1");
  scratch_arenas[1] = arena_alloc(Gigabytes_U64(1), "scratch arena 2");

  g_os_win32_state.is_initialised = true;
}

void os_win32_release()
{
  arena_release(scratch_arenas[0]);
  arena_release(scratch_arenas[1]); 
}

///////////////////////////////////////////////////////////
// Damian: Memory
//

OS_Win32_mem_reserve_result os_win32_mem_reserve(U64 base_address, U64 size_to_reserve)
{
  // I just for some reason couldnt make commit work with all these alignments and offsets
  NotImplemented();

  // TODO: Deal with the first parameter for Virtuall Alloc
  OS_Win32_mem_reserve_result result = {};
  {
    U64 size_to_reserve_rounded = align_up(size_to_reserve, g_os_win32_state.page_size); 
    U8* mem = (U8*)VirtualAlloc((void*)base_address, size_to_reserve_rounded, MEM_RESERVE, PAGE_NOACCESS);
    result.mem = mem;
    result.size = size_to_reserve_rounded;
  }
  return result;
  // TODO: Crash on the fail for this alloc
  //       Maybe show a win32 specific error window
}

U64 os_win32_mem_commit(U8* commit_region_base_address, U64 size_to_commit)
{
  // I just for some reason couldnt make commit work with all these alignments and offsets
  NotImplemented();

  U64 result_commited_size = 0;
  
  U8* base_address_rounded = align_up_p(commit_region_base_address, g_os_win32_state.page_size);  
  
  U64 size_to_commit_rounded = align_up(size_to_commit, g_os_win32_state.page_size);
  result_commited_size += size_to_commit_rounded;
  
  U8* mem = (U8*)VirtualAlloc(base_address_rounded, 
                              size_to_commit_rounded, 
                              MEM_COMMIT, PAGE_READWRITE);
  // TODO: Window pop up on fail for this
  return result_commited_size;
}

void os_win32_mem_release(U8* reserved_region_base_address)
{
  VirtualFree(reserved_region_base_address, Null, MEM_RELEASE);
  NotImplemented();
  // TODO: Window pop up on fail for this
}

#if OS_PROTECT
void os_win32_mem_protect(U8* base_address_of_region_to_protect, U64 size_to_protect)
{
  U8* base_address_rounded = os_win32_mem_round_up_address(base_address_of_region_to_protect, g_os_win32_state.page_size);
  U64 size_rounded = os_win32_mem_round_up_size(size_to_protect, g_os_win32_state.page_size);

  U32 prev_protection_type = {};
  BOOL succ = VirtualProtect(base_address_rounded, size_rounded, 
                             PAGE_NOACCESS, (unsigned long*)(&prev_protection_type));
  // TODO: See what do we do with this succ here

  // Round up the base address
  // Round up the size to protect
}
#endif

///////////////////////////////////////////////////////////
// Damian: Time
//
F64 os_win32_get_monotonic_time()
{
  F64 perf_counter = 0;
  {
    LARGE_INTEGER lr = {};
    QueryPerformanceCounter(&lr);
    perf_counter = (F64)lr.QuadPart;
  }

  F64 time_in_sec = perf_counter / g_os_win32_state.performance_freq_per_sec;
  return time_in_sec;
}

// void os_win32_get_system_time()
// {
//   SYSTEMTIME sys_time = {};
//   GetSystemTime(&sys_time);
// }

///////////////////////////////////////////////////////////
// Damian: Files
//  
OS_Win32_file os_win32_file_open(Str8 file_path, U32 access_flags)
{
  DWORD desired_access = 0;
  if (access_flags & File_access_flag_read) { desired_access |= GENERIC_READ; }
  if (access_flags & File_access_flag_write) { desired_access |= GENERIC_WRITE; }
  if (access_flags & File_access_flag_execute) { desired_access |= GENERIC_EXECUTE; }
  if (access_flags & File_access_flag_append) { desired_access |= GENERIC_WRITE; }
  
  DWORD share_mode = 0;
  if (access_flags & File_access_flag_share_read) { share_mode |= FILE_SHARE_READ; }
  if (access_flags & File_access_flag_share_write) { share_mode |= FILE_SHARE_WRITE; } 

  DWORD creation_parameters = 0;
  if (access_flags & File_access_flag_read) { creation_parameters = OPEN_EXISTING; }
  if (access_flags & File_access_flag_write) { creation_parameters = CREATE_ALWAYS; } // Truncates if exists
  if (access_flags & File_access_flag_append) { creation_parameters = OPEN_ALWAYS; }  // Doesnt truncate

  // Str8 file_path_null_term = str8_from_str8_temp_null_term(scratch.arena, file_path);
  Str8 file_path_null_term = str8_temp_from_str8(file_path);

  HANDLE file_handle = CreateFileA((CHAR *)file_path_null_term.data, 
                                   desired_access, 
                                   share_mode,
                                   Null,
                                   creation_parameters,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL);

  OS_Win32_file file = {};
  file.handle = file_handle; 

  return file;
}

void os_win32_file_close(OS_Win32_file file)
{
  CloseHandle(file.handle);
  file.handle = INVALID_HANDLE_VALUE;
}

B32 os_win32_is_file_valid(OS_Win32_file file)
{
  B32 is_valid = (file.handle != INVALID_HANDLE_VALUE); 
  return is_valid;
}

Data_buffer os_win32_file_read(Arena* arena, OS_Win32_file file)
{
  SetFilePointerEx(file.handle, LARGE_INTEGER{}, Null, FILE_BEGIN);

  U64 file_size = {};
  {
    LARGE_INTEGER lr = {};
    GetFileSizeEx(file.handle, &lr);
    file_size = lr.QuadPart;
  }

  Data_buffer buffer = data_buffer_make(arena, file_size);

  U64 bytes_read = 0;
  while (bytes_read < file_size)
  {
    U32 bytes_read_this_it = {};
    ReadFile(file.handle, buffer.data, file_size, (unsigned long*)&bytes_read_this_it, Null);
    bytes_read += bytes_read_this_it;
  }
  Assert(bytes_read == file_size);

  return buffer;
}

Data_buffer os_win32_file_read_inplace(Arena* arena, Str8 file_path)
{
  OS_Win32_file file = os_win32_file_open(file_path, File_access_flag_read);
  Data_buffer file_data = os_win32_file_read(arena, file);
  os_win32_file_close(file);
  return file_data;
}

void os_win32_file_write(OS_Win32_file file, Data_buffer buffer)
{
  SetFilePointerEx(file.handle, LARGE_INTEGER{}, Null, FILE_BEGIN);
  
  U64 bytes_written = 0;
  while (bytes_written < buffer.count)
  {
    U64 bytes_written_this_it = 0;    
    WriteFile(file.handle, buffer.data, buffer.count, (unsigned long*)&bytes_written_this_it, NULL);
    bytes_written += bytes_written_this_it;
  }
  Assert(bytes_written == buffer.count);
}

void os_win32_file_delete(Str8 file_name)
{
  // Str8 file_name_nt = str8_from_str8_temp_null_term(scratch.arena, file_name);
  Str8 file_name_nt = str8_temp_from_str8(file_name);
  DeleteFileA((const CHAR*)file_name_nt.data);
}

B32 os_win32_file_does_exist(Str8 file_name)
{
  // TODO: All these A path and names have to be converted to UTF-16 and used for W variants of win32 functions
  B32 does_exist = false;
  Str8 name_nt = str8_temp_from_str8(file_name);
  U32 attrs = GetFileAttributesA((const CHAR*)name_nt.data);
  does_exist = ((attrs != INVALID_FILE_ATTRIBUTES) && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
  return does_exist;
}

OS_Win32_file_data os_win32_file_data(OS_Win32_file file)
{ 
  SYSTEMTIME creation_time    = {};
  SYSTEMTIME last_access_time = {};
  SYSTEMTIME last_write_time  = {};
  {
    FILETIME f_creation_time    = {};
    FILETIME f_last_access_time = {};
    FILETIME f_last_write_time  = {};
    GetFileTime(file.handle, &f_creation_time, &f_last_access_time, &f_last_write_time);
    FileTimeToSystemTime(&f_creation_time, &creation_time);
    FileTimeToSystemTime(&f_last_access_time, &last_access_time);
    FileTimeToSystemTime(&f_last_write_time, &last_write_time);
  }

  U64 size = {};
  {
    LARGE_INTEGER lr = {};
    GetFileSizeEx(file.handle, &lr);
    size = lr.QuadPart;
  }

  OS_Win32_file_data result_data = {};
  result_data.creation_time    = creation_time;
  result_data.last_access_time = last_access_time;
  result_data.last_write_time  = last_write_time;
  result_data.size             = size;
  return result_data;
}

///////////////////////////////////////////////////////////
// Damian: Thread context
//
Arena* scratch_arenas[NumberOfScratchArenas];
U32 current_scratch_index = 0;

StaticAssert(ArrayCount(scratch_arenas) == 2, "get_scratch doesnt work if the number of scratch arenas doesnt equal 2.");
Scratch get_scratch()
{
  // IMPORTANT: I could also conditionally compile assert when debug mode
  //       and a fatal error window when release mode 

  Assert(g_os_win32_state.is_initialised);

  Assert(current_scratch_index == 0 || current_scratch_index == 1);
  current_scratch_index = (current_scratch_index == 0 ? 1 : 0);
  Arena* arena = scratch_arenas[current_scratch_index];
  Scratch scratch = temp_arena_begin(arena); 
  return scratch;  
}

void end_scratch(Scratch* scratch)
{ 
  temp_arena_end(scratch);
}

///////////////////////////////////////////////////////////
// Damian: Fatal error display
// 
void os_win32_exit(U32 exit_code)
{
  ExitProcess(exit_code);
}

void os_win32_display_fatal_error(const char* error_title, const char* error_text)
{
  MessageBoxA(
    NULL,                               
    error_title, 
    error_text,                      
    MB_ICONERROR|MB_OK               
  );
}








#endif