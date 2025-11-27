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

const char* month_as_cstr(Month month)
{
  const char* cstr = "";
  switch (month) 
  {
    default: { InvalidCodePath(); } break;
    case Month_January:   { cstr = "January";   } break;
    case Month_February:  { cstr = "February";  } break;
    case Month_March:     { cstr = "March";     } break;
    case Month_April:     { cstr = "April";     } break;
    case Month_May:       { cstr = "May";       } break;
    case Month_June:      { cstr = "June";      } break;
    case Month_July:      { cstr = "July";      } break;
    case Month_August:    { cstr = "August";    } break;
    case Month_September: { cstr = "September"; } break;
    case Month_October:   { cstr = "October";   } break;
    case Month_November:  { cstr = "November";  } break;
    case Month_December:  { cstr = "December";  } break;
  }  
  return cstr;
}

const char* day_as_cstr(Day day)
{
  const char* cstr = "";
  switch (day) 
  {
    default: { InvalidCodePath(); } break;
    case Day_Monday:    { cstr = "Monday";    } break;
    case Day_Tuesday:   { cstr = "Tuesday";   } break;
    case Day_Wednesday: { cstr = "Wednesday"; } break;
    case Day_Thursday:  { cstr = "Thursday";  } break;
    case Day_Friday:    { cstr = "Friday";    } break;
    case Day_Saturday:  { cstr = "Saturday";  } break;
    case Day_Sunday:    { cstr = "Sunday";    } break;
  }  
  return cstr;
}

Str8 time_as_str8(Arena* arena, Time time)
{
  Str8 result = str8_from_fmt_alloc(arena, "#U32:#U32", time.hour, time.minute);
  return result;
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



