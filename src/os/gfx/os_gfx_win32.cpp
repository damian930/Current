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

  // Crating an arena for events
  window->frame_event_arena = arena_alloc(Kilobytes_U64(64), "Widnow frame events arena");

  return window;
}

void win32_close_window(Win32_window* window)
{
  ReleaseDC(window->handle, window->hdc);
  DestroyWindow(window->handle);
  arena_release(window->frame_event_arena);
  
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
  B32 is_event = false;
  Event event = {};

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

      B32 got_pressed = (was_up && is_down);
      B32 got_released = (!was_up && !is_down);

      // Create a new event and push onto the windoes's event queue
      // so then the used can have more conctrol over how it gets processed

      switch (w_param)
      {
        default: {  } break;
        
        case 'W': 
        {
          is_event = true;
          event.type = Event_type_key;
          event.key  = Key_w;
          event.key_got_pressed = got_pressed;
          event.key_got_released = got_released;
        } break;

        case 'A': 
        {
          is_event = true;
          event.type = Event_type_key;
          event.key  = Key_a;
          event.key_got_pressed = got_pressed;
          event.key_got_released = got_released;
        } break;

        case 'S': 
        {
          is_event = true;
          event.type = Event_type_key;
          event.key  = Key_s;
          event.key_got_pressed = got_pressed;
          event.key_got_released = got_released;
        } break;

        case 'D': 
        {
          is_event = true;
          event.type = Event_type_key;
          event.key  = Key_d;
          event.key_got_pressed = got_pressed;
          event.key_got_released = got_released;
        } break;
      }
    } break;

    // WM_CHAR
    // WM_SYSKEYUP
    // WM_SYSKEYDOWN

    // #define WM_LBUTTONDBLCLK                0x0203
    // #define WM_RBUTTONDBLCLK                0x0206
    // #define WM_MBUTTONDBLCLK                0x0209

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_XBUTTONDOWN:
    // ---
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
    // ---
    case WM_MOUSEMOVE:
    {
      #define Message_type_up   1
      #define Message_type_down 2
      #define Message_type_move 3

      U32 message_type = Message_type_up;

      if ( message == WM_LBUTTONDOWN
        || message == WM_RBUTTONDOWN
        || message == WM_MBUTTONDOWN
        || message == WM_XBUTTONDOWN
      ) {
        message_type = Message_type_up;
      }

      if (message == WM_MOUSEMOVE) {
        message_type = Message_type_move;
      }

      Mouse_key mouse_key = Mouse_key_NONE;
      switch (message) 
      {
        default: {} break;
        case WM_LBUTTONUP: case WM_LBUTTONDOWN: { mouse_key = Mouse_key_left; } break;
        case WM_RBUTTONUP: case WM_RBUTTONDOWN: { mouse_key = Mouse_key_right; } break;
        case WM_MBUTTONUP: case WM_MBUTTONDOWN: { mouse_key = Mouse_key_middle; } break;
        case WM_XBUTTONUP: case WM_XBUTTONDOWN: { 
          if (w_param & MK_XBUTTON1) {
            mouse_key = Mouse_key_side_near; 
          }
          if (w_param & MK_XBUTTON2) {
            mouse_key = Mouse_key_side_far; 
          }
        } break;
      }

      B8 other_mouse_buttons[Mouse_key_COUNT] = {};
      
      other_mouse_buttons[Mouse_key_left] = (w_param & MK_LBUTTON);
      other_mouse_buttons[Mouse_key_right] = (w_param & MK_RBUTTON);
      other_mouse_buttons[Mouse_key_middle] = (w_param & MK_MBUTTON);
      other_mouse_buttons[Mouse_key_side_far] = (w_param & MK_XBUTTON2);
      other_mouse_buttons[Mouse_key_side_near] = (w_param & MK_XBUTTON1);
      
      S32 x = (S16)LOWORD(l_param);
      S32 y = (S16)HIWORD(l_param);
      
      is_event = true;
      event.type = Event_type_mouse;
      event.mouse_x = x;
      event.mouse_y = y;
      event.mouse_key_released = mouse_key;
      
      if (message_type == Message_type_up) {
        MemCopy(event.other_released_mouse_buttons, other_mouse_buttons, sizeof(other_mouse_buttons));
      } else {
        MemCopy(event.other_pressed_mouse_buttons, other_mouse_buttons, sizeof(other_mouse_buttons));
      }
      
      StaticAssert(sizeof(other_mouse_buttons)     == sizeof(Event::other_pressed_mouse_buttons), "These are supposed to be the same to do a safe MemCopy on them");
      StaticAssert(ArrayCount(other_mouse_buttons) == ArrayCount(Event::other_pressed_mouse_buttons), "These are supposed to be the same to do a safe MemCopy on them");
    
      StaticAssert(sizeof(other_mouse_buttons)     == sizeof(Event::other_released_mouse_buttons), "These are supposed to be the same to do a safe MemCopy on them");
      StaticAssert(ArrayCount(other_mouse_buttons) == ArrayCount(Event::other_released_mouse_buttons), "These are supposed to be the same to do a safe MemCopy on them");
    
      #undef Message_type_up
      #undef Message_type_down
      #undef Message_type_move
    
    } break;

    case WM_MOUSEWHEEL:
    {
      S32 wheen_rotation = (S16)HIWORD(w_param);
      S32 other_mouse_keys_whitch_are_down = (S16)LOWORD(w_param);

      S32 x = (S16)LOWORD(l_param);
      S32 y = (S16)HIWORD(l_param);

      is_event = true;
      event.mouse_x = x;
      event.mouse_y = y;
      event.wheen_rotation = wheen_rotation;
    }

    case WM_SYSCOMMAND:
    {
      // Damian: (w_param & 0xFFF0) is from the example for WM_SYSCOMMAND.
      //         Some bits are reserved.
      switch (w_param & 0xFFF0)
      {
        default: { result = DefWindowProc(window_handle, message, w_param, l_param); } break;

        case SC_MINIMIZE:
        {
          event.type = Event_type_minimize;
        } break;

        case SC_MAXIMIZE:
        {
          event.type = Event_type_maximize;
        } break;
      }
    } break;

    // Asked Claude for some general things people handle
    // -- Window State Events
    // 
    // Window minimize/maximize/restore
    // Window close/destroy
    // Window resize
    // Window move
    // Window activate/deactivate (gaining/losing focus)
    // Window show/hide

    // Keyboard Events

    // Key press (key down)
    // Key release (key up)
    // Character input (for text entry, handles things like Shift+key combinations)
    // System key events (Alt key combinations, F10, etc.)

    // Mouse Events

    // Mouse button press (left, right, middle button down)
    // Mouse button release (left, right, middle button up)
    // Mouse double-click
    // Mouse move
    // Mouse wheel scroll
    // Mouse enter/leave window area
    // Mouse hover

    // Focus Events

    // Control/window gaining focus
    // Control/window losing focus
    // Tab key navigation between controls

    // Menu & Command Events

    // Menu item selection
    // Accelerator/shortcut key activation
    // Context menu requests (right-click)

    // Timer Events

    // Timer tick/elapsed

    // Clipboard Events

    // Clipboard content changed
    // Cut/copy/paste operations

    // Drag & Drop Events

    // Drag enter
    // Drag over
    // Drag leave
    // Drop
  }

  // Storing the event 
  if (is_event) 
  {
    Win32_window* window = win32_window_from_win32_handle(window_handle);
    Arena* frame_event_arena = window->frame_event_arena;
    Event_list* event_list = window->frame_event_list;
    Event_node* node = ArenaPush(frame_event_arena, Event_node);
    node->event = event;
    DllPushBack(event_list, node);
    event_list->count += 1;
  }
  
  return result;
}

// B32 TEST_was_key_pressed(Key key)
// { 
//   B32 did_it_happen = false;
//   for (Event_node* node = event_list.first; node != 0; node = node->next)
//   {
//     if (node->event.key == key && node->event.was_up && node->event.is_down) {
//       did_it_happen = true;
//       DllPopNode(&event_list, node);
//       event_list.count -= 1;
//     }
//   }
//   return did_it_happen;
// }

// B32 TEST_is_key_down(Key key)
// {
//   B32 did_it_happen = false;
//   for (Event_node* node = event_list.first; node != 0; node = node->next)
//   {
//     if (node->event.key == key && node->event.is_down) {
//       did_it_happen = true;
//       DllPopNode(&event_list, node);
//       event_list.count -= 1;
//     }
//   }
//   return did_it_happen;
// }





















#endif
