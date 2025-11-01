#ifndef OS_GFX_WIN32_CPP
#define OS_GFX_WIN32_CPP

#include "os_gfx_win32.h"

#define Win32WindowClassName L"Flopper Window Class"

///////////////////////////////////////////////////////////
// Damian: Globals
extern Win32_gfx_state g_win32_gfx_state = {};

void win32_gfx_init()
{
  g_win32_gfx_state = Win32_gfx_state{};
  g_win32_gfx_state.window_arena = arena_alloc(Kilobytes_U64(100), "Win32 gfx state window arena"); 
  g_win32_gfx_state.windows_buffer = (Win32_window*)arena_first_usable_byte(g_win32_gfx_state.window_arena);
  
  WNDCLASSEXW wc = {}; 
  wc.cbSize        = sizeof(WNDCLASSEXW); 
  wc.style         = CS_HREDRAW | CS_VREDRAW; 
  wc.lpfnWndProc   = WndProc; 
  wc.hInstance     = GetModuleHandleW(Null); 
  wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW); 
  wc.lpszClassName = Win32WindowClassName; 
  RegisterClassExW(&wc); // TODO: Do we assert here
}  

void win32_gfx_release()
{
  // NOTE: Module handle doesnt have to be cleared if was retrived using GetModuleHandle,
  //       since it doesnt increment the internal win32 handle counter.
  arena_release(g_win32_gfx_state.window_arena);
  g_win32_gfx_state = Win32_gfx_state{};
}

Win32_window* win32_create_window()
{
  // TODO: See what do we want to do with init state checking here. 
  // IDEAS: Assert, Nothing hanppends_everything is protected and warning is logged to the developer logger or something. 
  
  // Allocating the new window
  Win32_window* window = g_win32_gfx_state.first_free;
  if (window) {
    *window = Win32_window{}; 
    g_win32_gfx_state.first_free = g_win32_gfx_state.first_free->next_free;
    g_win32_gfx_state.window_count += 1;
    g_win32_gfx_state.free_window_count -= 1;
  } else {
    window = ArenaPush(g_win32_gfx_state.window_arena, Win32_window);
    g_win32_gfx_state.window_count += 1;
  }
  
  window->handle = CreateWindowExW(
    0L,
    Win32WindowClassName,
    L"Window",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    800,
    600,
    Null,
    Null,
    GetModuleHandleW(Null),
    Null);
  window->hdc = GetDC(window->handle);

  ShowWindow(window->handle, SW_NORMAL);
  UpdateWindow(window->handle);

  return window;
}

void win32_close_window(Win32_window* window)
{
  ReleaseDC(window->handle, window->hdc);
  DestroyWindow(window->handle);
  *window = Win32_window{};

  window->is_free = true;
  StackPush_Name(&g_win32_gfx_state, window, first_free, next_free);
  g_win32_gfx_state.window_count -= 1;
  g_win32_gfx_state.free_window_count += 1;
}

B32 win32_window_shoud_close(Win32_window* window)
{
  B32 result = window->shoud_be_closed;
  return result;
}

// TODO: See if this is needed, this return the client rect on screen
Rect _win32_get_client_arena_rect(Win32_window* window)
{
  RECT rect = {};
  GetClientRect(window->handle, &rect);
  Rect result = {};
  result.x = rect.right;
  result.y = rect.top;
  result.width = rect.right - rect.left;
  result.height = rect.bottom - rect.top;
  return result;
}

Rect win32_get_client_area_rect(Win32_window* window)
{
  RECT rect = {};
  GetClientRect(window->handle, &rect);
  Rect result = {};
  result.x = 0;
  result.y = 0;
  result.width = rect.right - rect.left;
  result.height = rect.bottom - rect.top;
  return result;
}

Rect win32_get_window_rect(Win32_window* window)
{
  RECT rect = {};
  GetWindowRect(window->handle, &rect);
  Rect result = {};
  result.x = 0;
  result.y = 0;
  result.width = rect.right - rect.left;
  result.height = rect.bottom - rect.top;
  return result;
}

Rect win32_get_screen_rect()
{
  S32 primary_monitor_width_px = GetSystemMetrics(SM_CXSCREEN);
  S32 primary_monitor_height_px = GetSystemMetrics(SM_CYSCREEN);
  Rect rect = {};
  rect.x = 0.0f;
  rect.y = 0.0f;
  rect.width  = (F32)primary_monitor_width_px;
  rect.height = (F32)primary_monitor_height_px;
  return rect;
}

LRESULT CALLBACK WndProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
  U64 result = 0;
  switch (message)
  {
    default: { 
      result = DefWindowProc(window_handle, message, w_param, l_param);
    } break;

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window_handle, &ps);
      EndPaint(window_handle, &ps);
    } break;

    case WM_SIZE:
    {
      Scratch scratch = get_scratch();
      end_scratch(&scratch);
    } break;

    case WM_MOVE:
    {
      Scratch scratch = get_scratch();
      end_scratch(&scratch);
    } break;

    // NOTE: This messege is received when the app is to be closed.
    //       For example after the used presses the close button for the window
    //       When this is received, the window is still alive, its just a message for us to know,
    //       that the used want this window to be closed.
    case WM_CLOSE: 
    {
      Win32_window* window = 0;
      U64 windows_in_total = g_win32_gfx_state.window_count + g_win32_gfx_state.free_window_count;
      for (U64 window_index = 0; window_index < windows_in_total; window_index += 1)
      {
        Win32_window* test_window = g_win32_gfx_state.windows_buffer + window_index;
        if (test_window->handle == window_handle) {
          window = test_window;
          break;
        }
      } 
  
      if (!window->is_free) {
        window->shoud_be_closed = true;
      }
    } break;

    // NOTE: This messege is received after the window is gone, its not alive any mode.
    //       This is sent by the system afther the DestroyWindow() call. 
    //       So its more like a callback for us.
    case WM_DESTROY:
    {
      // Nothing here
    } break;

    // ==================================================================================
    // ----------- Inputs ---------------------------------------------------------------

    case WM_KEYUP:
    case WM_KEYDOWN:
    {
      U32 repeat_count = ((l_param >> 0) & 0b111);
      U32 scan_code = ((l_param >> 16) & 0b11111111);
      U32 extended_key_flag = ((l_param >> 24) & 0b1);
      U32 context_code = ((l_param >> 29) & 0b1);

      B32 was_up = ((l_param >> 30) & 0b1) == 0;
      B32 is_down = ((l_param >> 31) & 0b1) == 0;

      // Create a new event and push onto the windoes's event queue
      // so then the used can have more conctrol over how it gets processed

      B32 is_event = false;
      Event event = {};
      
      switch (w_param)
      {
        default: {  } break;
        
        case 'A': 
        {
          is_event = true;
          event.key      = Key_a;
          event.was_up   = was_up;
          event.was_down = !was_up;
          event.is_up    = !is_down;
          event.is_down  = is_down;
        } break;
      }

      if (is_event) {
        Event_node* node = ArenaPush(event_arena, Event_node);
        node->event = event;
        DllPushBack(&event_list, node);
        event_list.count += 1;
      }

    } break;

    // WM_CHAR
    // WM_SYSKEYUP
    // WM_SYSKEYDOWN

  }
  
  return result;
}

B32 TEST_was_key_pressed(Key key)
{ 
  B32 did_it_happen = false;
  for (Event_node* node = event_list.first; node != 0; node = node->next)
  {
    if (node->event.key == key && node->event.was_up && node->event.is_down) {
      did_it_happen = true;
      DllPopNode(&event_list, node);
      event_list.count -= 1;
    }
  }
  return did_it_happen;
}

B32 TEST_is_key_down(Key key)
{
  B32 did_it_happen = false;
  for (Event_node* node = event_list.first; node != 0; node = node->next)
  {
    if (node->event.key == key && node->event.is_down) {
      did_it_happen = true;
      DllPopNode(&event_list, node);
      event_list.count -= 1;
    }
  }
  return did_it_happen;
}





















#endif
