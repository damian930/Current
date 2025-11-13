#ifndef WIN32_CORE_H
#define WIN32_CORE_H

#include "base/core.h"
#include "base/string.h"
#include "base/arena.h"

#include <windows.h>

struct OS_Win32_mem_reserve_result {
  U8* mem;
  U64 size;
};

enum File_access_flags : U32 {
  File_access_flag_read        = (1 << 0),
  File_access_flag_write       = (1 << 1),
  File_access_flag_execute     = (1 << 2),
  File_access_flag_append      = (1 << 3),
  File_access_flag_share_read  = (1 << 4),
  File_access_flag_share_write = (1 << 5),
};

struct OS_Win32_file_data {
  SYSTEMTIME creation_time;
  SYSTEMTIME last_access_time;
  SYSTEMTIME last_write_time;
  U64 size;
};

struct OS_Win32_file {
  HANDLE handle;
};

// Damian: Usually when i have layers and states for them i like to allocated them on their own lifetime.
//         I need an allocated for that (Arena). But i cant do it here, since i need a way to allocate.
//         I dont have that ability yet, before i have initialised the os_core layer.
//         I could use malloc for it, but for now i dont. State only has static values either way (at least for now).
struct OS_Win32_state
{
  F64 performance_freq_per_sec;
  U32 page_size;
  U32 allocation_granularity;
};

// TODO: Implement errors and asserts and exits for the app when this layer is used before beeing initilised
//       Or just have it not crash, but also not work.
global OS_Win32_state g_os_win32_state = {};

// State
void os_win32_init();
void os_win32_release();

// Memory
// TODO: Alignment for allocations
OS_Win32_mem_reserve_result os_win32_mem_reserve(U64 base_address, U64 size_to_reserve);
U64 os_win32_mem_commit(U8* base_address_of_regioun_to_commit, U64 size_to_commit);
void os_win32_mem_release(U8* reserved_region_base_address);
#if OS_PROTECT
void os_win32_mem_protect(U8* base_address_of_region_to_protect, U64 size_to_protect);
#endif

// Time
F64 os_win32_get_monotonic_time();

// Files
OS_Win32_file os_win32_file_open(Str8 file_path, U32 access_flags);
void os_win32_file_close(OS_Win32_file file);
B32 os_win32_file_is_valid(OS_Win32_file file);
Data_buffer os_win32_file_read(Arena* arena, OS_Win32_file file);
Data_buffer os_win32_file_read_inplace(Arena* arena, Str8 file_path);
void os_win32_file_write(OS_Win32_file file, Data_buffer buffer);
void os_win32_file_delete(Str8 file_name);
OS_Win32_file_data os_win32_file_data(OS_Win32_file file);

// #define DefereFileLoop(var_name, file_name, file_flags) \
//   OS_Win32_file file_name = os_win32_file_open(file_name, file_flags);

// Thread context
#define NumberOfScratchArenas 2
extern Arena* scratch_arenas[NumberOfScratchArenas];  
extern U32 current_scratch_index;

typedef Temp_arena Scratch;
Scratch get_scratch();
void end_scratch(Scratch* scratch);

// Manuall exit strategies
void os_win32_exit(U32 exit_code);  
void os_win32_display_fatal_error(const char* error_title, const char* error_text);




#endif
















