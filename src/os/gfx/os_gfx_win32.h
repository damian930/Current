#ifndef OS_GFX_WIN32_H
#define OS_GFX_WIN32_H

#include <windows.h>
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Gdi32.lib")

#include "base/core.h"
#include "base/arena.h"
#include "base/arena.cpp"

#include "math/math.h"
#include "math/math.cpp"

// -------------------------
enum Key_state {
  Key_state_up,
  Key_state_down,
};

enum Key {
  Key_NONE,
  Key_a,
  Key_COUNT,
};

struct Event {
  Key key;
  B32 was_up;
  B32 was_down;
  B32 is_up;
  B32 is_down;
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

global Arena* event_arena = arena_alloc(Kilobytes_U64(10), "Event arena for win32");
global Event_list event_list = {};
// -------------------------

struct Win32_window {
  B32 is_free;
  Win32_window* next_free;

  B32 shoud_be_closed;
  HWND handle;
  HDC hdc;
};

struct Win32_gfx_state {
  Arena* window_arena;
  Win32_window* windows_buffer;
  Win32_window* first_free;
  U64 window_count;
  U64 free_window_count;
};

///////////////////////////////////////////////////////////
// Damian: Globals
extern Win32_gfx_state g_win32_gfx_state;

void win32_gfx_init();
void win32_gfx_release();

Win32_window* win32_create_window();
void win32_close_window(Win32_window* window);

B32 win32_window_shoud_close(Win32_window* window);

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