#ifndef D_PROCESS_TRACKER_H
#define D_PROCESS_TRACKER_H

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

#include "os/core/os_core_win32.h"
#include "os/core/os_core_win32.cpp" // TODO: Remove this from here

#include <tlhelp32.h>	
#include <psapi.h>
#pragma comment(lib, "Kernel32.lib")

// TODO: Just do this with the things i need here, then split this into the os extended feature set

///////////////////////////////////////////////////////////
// Damian: This is Win32 specific stuff. 
//         This is here for me to just see how i want to use it and what i want to use.
//         When i am dont with it, i will add it to the os specific stuff and generalise.
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

Win32_snapshot_process_list* win32_get_snapshot_and_process_handles(Arena* arena);
void win32_release_snapshot_and_process_handles(Win32_snapshot_process_list* list);

// ------

enum Month {
  Month_January,
  Month_February,
  Month_March,
  Month_April,
  Month_May,
  Month_June,
  Month_July,
  Month_August,
  Month_September,
  Month_October,
  Month_November,
  Month_December,
  Month_Count,
};

enum Day {
  Day_Monday,
  Day_Tuesday,
  Day_Wednesday,
  Day_Thursday,
  Day_Friday,
  Day_Saturday,
  Day_Sunday,
  Day_Count
};

struct Time { // UTC
  U32 year;
  Month month;
  Day day;
  U32 hour;
  U32 minute;
  U32 second;
  U32 millie_second;
};

Time time_from_win32_system_time(SYSTEMTIME* sys_time)
{
  Time time = {};
  time.year          = sys_time->wYear;
  time.month         = (Month)sys_time->wMonth;
  time.day           = (Day)sys_time->wDay;
  time.hour          = sys_time->wHour;
  time.minute        = sys_time->wMinute;
  time.second        = sys_time->wSecond;
  time.millie_second = sys_time->wMilliseconds;
  return time;
}

///////////////////////////////////////////////////////////
// Damian: Generic process data stuff 
//
struct Process_data {
  Str8 path;
  Time creation_time; // TODO: This is win32 specific
};

struct Process_data_node {
  Process_data_node* next;
  Process_data_node* prev;
  Process_data process_data;
};

struct Process_data_list {
  Process_data_node* first;
  Process_data_node* last;
  U32 count;
};

Process_data_list* get_all_process_data(Arena* arena);


#endif



