#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

#include <windows.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")

#include "base/core.h"
#include "base/arena.h"
#include "base/arena.cpp"

#include "base/math.h"
#include "base/math.cpp"

// -------------------------
// enum Key_state {
//   Key_state_up,
//   Key_state_down,
// };

enum Key {
  Key_NONE,
  Key_w,
  Key_s,
  Key_a,
  Key_d,
  Key_COUNT,
};

enum Event_type {
  Event_type_NONE,
  Event_type_minimize,
  Event_type_maximize,
  Event_type_key,
  Event_type_mouse,
};

enum Mouse_key {
  Mouse_key_NONE,
  Mouse_key_left,
  Mouse_key_right,
  Mouse_key_middle,
  Mouse_key_side_far,
  Mouse_key_side_near,
  Mouse_key_COUNT,
};

// TODO: I have made a mistacke where i was trying to get keyboard inputs for mouse,
//       cause they are inside the same struct and names are similar,
//       fix that, make it now happen
// TODO: Change some of these S32 to S16
struct Event {
  Event_type type;
  
  Key key;
  B32 key_got_pressed;
  B32 key_got_released;

  S32 mouse_x;
  S32 mouse_y;

  Mouse_key mouse_key_pressed;
  B8 other_pressed_mouse_buttons[Mouse_key_COUNT];
  // TODO: I dont like this duplication here 
  Mouse_key mouse_key_released;
  B8 other_released_mouse_buttons[Mouse_key_COUNT];

  S16 wheen_rotation;
};

struct Event_node {
  Event_node* next;
  Event_node* prev;
  Event event;
};

struct Event_list {
  Event_node* first;
  Event_node* last;
  U64 count;
};

struct Win32_window {
  // Free list stuff
  B32 is_free;
  Win32_window* next_free;
  
  // Persistent stuff
  B32 shoud_be_closed;
  HWND handle;
  HDC hdc;

  // Per frame stuff
  Arena* frame_event_arena;
  Event_list* frame_event_list;
  Vec2* frame_final_mouse_pos;

  // TODO: Thinkg about this here, this was just added cause i needed it
  Vec2 last_frame_final_mouse_pos;
};

struct Win32_gfx_state {
  Arena* window_arena;
  Win32_window* windows_buffer;
  Win32_window* first_free;
  U64 window_count;
  U64 free_window_count;
};

// --DEBUG-----------------

// NOTE: This is something that i am thinking about doing.
//       Like a nice way to get input to to the called, 
//       os events are just events (raw), this will be the more usable nice version

// struct Key_nice_data {
//   Key key;
//   B32 is_up;
// };
// static Key_nice_data a_key_nice_data = {Key_a, false};

// void DEBUG_get_nice_for_a(Win32_window* window)
// {
//   for (Event_node* node = window->frame_event_list->first; node != 0; node = node->next)
//   {
//     Event* event = window->frame_event_list->first;
//     if (event.)
//   }
// }

B32 is_key_clicked(Win32_window* window, Key key)
{
  B32 result = false;
  for (Event_node* node = window->frame_event_list->first; node != 0; node = node->next)
  {
    Event* event = &node->event;
    if (event->key == key && event->key_got_released)
    {
      result = true;
      break;
    }    
  }
  return result;
}

U8 key_to_char(Key key)
{
  U8 ch = '\0';
  switch (key)
  {
    default:        { InvalidCodePath("Handle all the keys."); } break;
    case Key_NONE:  {} break;
    case Key_w:     { ch = 'w'; } break;
    case Key_s:     { ch = 's'; } break;
    case Key_a:     { ch = 'a'; } break;
    case Key_d:     { ch = 'd'; } break;
    case Key_COUNT: {} break;
  }
  return ch;
}

// B32 is_key_down(Win32_window* window, Key key)
// {

// }
// ------------------------


///////////////////////////////////////////////////////////
// Damian: Globals
//
extern Win32_gfx_state g_win32_gfx_state;

Win32_window* win32_window_from_win32_handle(HWND window_handle)
{
  Win32_window* result = 0;
  U64 max_windows = g_win32_gfx_state.window_count + g_win32_gfx_state.free_window_count;
  ForEachEx(window_index, max_windows, g_win32_gfx_state.windows_buffer)
  {
    Win32_window* test_window = g_win32_gfx_state.windows_buffer + window_index;
    if (test_window->handle == window_handle)
    {
      result = test_window;
      break;
    }
  } 
  return result;
}

void win32_gfx_init();
void win32_gfx_release();

Win32_window* win32_create_window();
void win32_close_window(Win32_window* window);

B32 win32_window_shoud_close(Win32_window* window);

// Damian: This just calls the winproc. 
//         Winproc then captures all the event we need into the frame event list.
void win32_handle_messages(Win32_window* window) 
{
  MSG message = {}; 
  while (PeekMessage(&message, window->handle, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}
LRESULT CALLBACK WndProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

Rect win32_get_client_area_rect(Win32_window* window);
Rect win32_get_window_rect(Win32_window* window);
Rect win32_get_screen_rect(); // TODO: I dont know which screen this is 

// TODO: Need a better name for this
#define CreateCustonWindowDrawF(name) void name()
typedef CreateCustonWindowDrawF(window_draw_ft);
window_draw_ft* window_draw_fp = 0;

// CreateCustonWindowDrawF(window_draw_fp_placeholder) {
//   glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
//   glClear(GL_COLOR_BUFFER_BIT);
// }
// window_draw_ft* window_draw_fp = window_draw_fp_placeholder;

void win32_gfx_set_draw_f(window_draw_ft* draw_f) {
  window_draw_fp = draw_f;
}

// ---------------------------------------


// B32 win32_window_shoud_close()
// {
//   Assert(g_win32_gfx_state.is_window_created);
//   B32 result = !g_win32_gfx_state.window.is_running;
//   return result;
// }

// void win32_handle_messages()
// {
//   
// }

#define SCREEN_SHOT_CODE 0
#if SCREEN_SHOT_CODE

Data_buffer win32_make_screen_shot(Arena* arena)
{
  Data_buffer result_buffer = {};
  
  HDC screen_dc = {};
  DefereLoop(screen_dc = GetDC(Null), ReleaseDC(Null, screen_dc))
  {
    // Get the screen handle
    // Create compatible mem dc for the screen handle
    // Create bitmap for the screen handle
    // Select the bitmap to but used
    // Copy the data fron the screen handle to the bitmap handle 
    // Get the bits from the bitmap 
    // Return the data 
    
    // Creating a new dc and a bitmap for it 
    Rect screen_rect = win32_get_screen_rect();
    
    
    HDC screen_mem_dc = {};
    DefereLoop(screen_mem_dc = CreateCompatibleDC(screen_dc), DeleteDC(screen_mem_dc))
    {
      HBITMAP screen_mem_bitmap_handle = {};
      DefereLoop(screen_mem_bitmap_handle = CreateCompatibleBitmap(screen_dc, screen_rect.width, screen_rect.height),
                 DeleteObject(screen_mem_bitmap_handle)
      ) {
        SelectObject(screen_mem_dc, screen_mem_bitmap_handle);

        // Copying the bitmap data from the screen dc to the one we just created 
        BitBlt(screen_mem_dc,
               0, 0,
               screen_rect.width, screen_rect.height,
               screen_dc,
               0, 0,
               SRCCOPY);
    
        BITMAP bitmap = {};
        GetObjectW(screen_mem_bitmap_handle, sizeof(BITMAP), &bitmap);
    
        BITMAPINFOHEADER bi = {};
        bi.biSize        = sizeof(BITMAPINFOHEADER);
        bi.biWidth       = bitmap.bmWidth;
        bi.biHeight      = bitmap.bmHeight;  // Negative for top-down
        bi.biPlanes      = bitmap.bmPlanes;
        bi.biBitCount    = bitmap.bmBitsPixel;
        bi.biCompression = BI_RGB;
    
        // TODO: Make this formula better
        U64 bitmap_size  = ((bitmap.bmWidth * bi.biBitCount + 31) / 32) * 4 * bitmap.bmHeight;
        
        U64 file_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bitmap_size;
        
        BITMAPFILEHEADER bmfHeader = {};
        bmfHeader.bfType    = 0x4D42; // 'BM'
        bmfHeader.bfSize    = file_size;
        bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
        result_buffer.data  = ArenaPushArr(arena, U8, file_size);
        result_buffer.count = file_size;
    
        // Copying the memory over from the bitmap handle to the buffer
        {
          U8* ptr = result_buffer.data;
          MemCopy(ptr, &bmfHeader, sizeof(bmfHeader));
          ptr += sizeof(bmfHeader);
          MemCopy(ptr, &bi, sizeof(bi));
          ptr += sizeof(bi);
          GetDIBits(screen_dc, screen_mem_bitmap_handle, 0, bitmap.bmHeight,
                    ptr, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
        }

      }
    }
  }
  return result_buffer;
}


#endif // SCREEN_SHOT_CODE










#endif