#ifndef RENDER_GL_WIN32_CPP
#define RENDER_GL_WIN32_CPP

#include "render/render_gl_win32.h"
#include "os/core/os_core_win32.h" // TODO: See maybe can pull this into the header file instead of having it here


// TODO: Improve th loading for the context
//       Fake window
//       Callback
//       Better code for shaders
//       *Try to then use the ARB extension, see if it changes anything
//       *Do some with the versions for opengl, those are weird

///////////////////////////////////////////////////////////
// Damian: Opengl state globals
//
#define R_WIN32_GL_DEFAULT_FRAME_RATE ((U32)60)
#define R_WIN32_GL_DEFAULT_IS_HEIGHT_BOTTOM_UP false

HWND gl_context_fake_window_handle = 0;
HDC gl_context_fake_window_hdc     = 0;
HGLRC gl_context                   = 0;
GL_renderer* g_win32_gl_renderer   = 0;

// TODO: Maybe move these to header with extern
file_private B32 is_rect_program_loaded = false;
file_private GLuint rect_vbo_id         = 0;
file_private GLuint rect_ebo_id         = 0;
file_private GLuint rect_vao_id         = 0; 
file_private GLuint rect_program_id     = 0;

void gl_win32_init_helper()
{
  // Creating a fake window context to get the loading functions for the real context
  WNDCLASSEXW fake_window_class = {};
  HWND fake_window_handle       = {};
  HDC fake_hdc                  = {};
  HGLRC fake_gl_context         = {};
  { 
    // Damian: A predefine win32 class might also be used here, 
    //         eg: Static --> https://learn.microsoft.com/en-us/windows/win32/winmsg/about-window-classes
    fake_window_class.cbSize        = sizeof(WNDCLASSEXW); 
    fake_window_class.style         = CS_HREDRAW | CS_VREDRAW; 
    fake_window_class.lpfnWndProc   = DefWindowProc; 
    fake_window_class.hInstance     = GetModuleHandleW(Null); 
    fake_window_class.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
    fake_window_class.hCursor       = LoadCursor(NULL, IDC_ARROW); 
    fake_window_class.lpszClassName = L"Fake window for gl loading"; 
    RegisterClassExW(&fake_window_class);
    
    fake_window_handle = CreateWindowExW(
      0L, fake_window_class.lpszClassName, L"Fake window",
      WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
      Null, Null, Null, Null
    );
    
    fake_hdc = GetDC(fake_window_handle);

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32; 
    pfd.cDepthBits   = 24; 
    pfd.cStencilBits = 8;  
    pfd.iLayerType   = PFD_MAIN_PLANE; 

    int pf = ChoosePixelFormat(fake_hdc, &pfd);
    BOOL describe = DescribePixelFormat(fake_hdc, pf, sizeof(pfd), &pfd);
    BOOL set_pf = SetPixelFormat(fake_hdc, pf, &pfd);

    fake_gl_context = wglCreateContext(fake_hdc);
    wglMakeCurrent(fake_hdc, fake_gl_context); 

    // Loadint extensions
    {
      wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC_T*) wglGetProcAddress("wglGetExtensionsStringARB");
      Assert(wglGetExtensionsStringARB); // TODO: This has to load

      // Damian: Not matching if extension are present, since i expect them to be present
      // const char* ext = wglGetExtensionsStringARB(fake_hdc);
      wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC_T*)    wglGetProcAddress("wglChoosePixelFormatARB");
      wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC_T*) wglGetProcAddress("wglCreateContextAttribsARB");
      
      Assert(wglChoosePixelFormatARB);
      Assert(wglCreateContextAttribsARB);
    }

    // Clear all the stuff
    wglMakeCurrent(fake_hdc, Null);
    wglDeleteContext(fake_gl_context);

    // Damian: I am not clearing this, since i want to load all the stuff for gl usage at layer init.
    //         Technically i am "leacking" the window to the system. 
    //         But its fine, this way i can easily just load it all here,
    //         which otherwise i dont see where to comfortably do without some static(local) 
    //         one per runtime loading.
    // ReleaseDC(fake_window_handle, fake_hdc);
    // DestroyWindow(fake_window_handle);
    // UnregisterClassW(fake_window_class.lpszClassName, GetModuleHandleW(Null));
  }

  // Creating the real context 
  HGLRC real_context = {};
  {
    int pf = 0;
    UINT num_formats = 0;
    int pf_attrs[] =
    {
      WGL_DRAW_TO_WINDOW_ARB, 1,
      WGL_SUPPORT_OPENGL_ARB, 1,
      WGL_DOUBLE_BUFFER_ARB, 1,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 32,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,
      0
    };
    wglChoosePixelFormatARB(fake_hdc, pf_attrs, 0, 1, &pf, &num_formats);

    // Loading gl functions
    int attrib[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      #if DEBUG_MODE
        // ask for debug context for non "Release" builds
        // this is so we can enable debug callback
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
      #endif
        0,
    };
    real_context = wglCreateContextAttribsARB(fake_hdc, NULL, attrib);
    BOOL ok = wglMakeCurrent(fake_hdc, real_context);

    // Loading newer gl funcs
    #define GL_FUNC_EXP(Type, name, parameters) name = (name##_FuncType*)r_gl_win32_load_extension_functions_opt(#name);
      GL_FUNC_TABLE
    #undef GL_FUNC_EXP
    
    #define GL_FUNC_EXP(Type, name, parameters) Assert(name);
      GL_FUNC_TABLE
    #undef GL_FUNC_EXP

    #if DEBUG_MODE
      // enable debug callback
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(r_gl_debug_message_callback, NULL);
    #endif
  }

  gl_context_fake_window_handle = fake_window_handle;
  gl_context_fake_window_hdc    = fake_hdc;
  gl_context                    = real_context;

  gl_load_rect_program();

  // Damian: The fake window and the hdc for it are leacked here.
  //         The contect created get invalidated if the dc for which it is context gets released.
  //         
  // TODO: Maybe store the hdc just to have it clear that i leack it (kind of),
  //       then remove it when render layer state is released.
}

// ----

void r_gl_win32_state_init()
{
  Assert(g_win32_gl_renderer == 0);
  gl_win32_init_helper();

  Arena* arena = arena_alloc(Kilobytes_U64(10), "Renderer GL state arena");
  g_win32_gl_renderer = ArenaPush(arena, GL_renderer);
  g_win32_gl_renderer->state_arena = arena;

  g_win32_gl_renderer->frame_rate = R_WIN32_GL_DEFAULT_FRAME_RATE;

  g_win32_gl_renderer->frame_arena = arena_alloc(Kilobytes_U64(100), "Renderer GL frame arena");
}

void r_gl_win32_state_release()
{
  wglMakeCurrent(gl_context_fake_window_hdc, Null);
  wglDeleteContext(gl_context);
  gl_context = 0;

  ReleaseDC(gl_context_fake_window_handle, gl_context_fake_window_hdc);
  DestroyWindow(gl_context_fake_window_handle);
  gl_context_fake_window_hdc = 0;
  gl_context_fake_window_handle = 0;

  arena_release(g_win32_gl_renderer->state_arena);
  g_win32_gl_renderer = 0;
}

///////////////////////////////////////////////////////////
// Damian: Opengl window
//
void r_gl_win32_equip_window(Win32_window* window)
{
  HDC hdc = window->hdc;
  g_win32_gl_renderer->window = window;

  wglMakeCurrent(hdc, gl_context);

  int pf = 0;
  UINT num_formats = 0;
  int pf_attrs[] =
  {
    WGL_DRAW_TO_WINDOW_ARB, 1,
    WGL_SUPPORT_OPENGL_ARB, 1,
    WGL_DOUBLE_BUFFER_ARB, 1,
    WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
    WGL_COLOR_BITS_ARB, 32,
    WGL_DEPTH_BITS_ARB, 24,
    WGL_STENCIL_BITS_ARB, 8,
    0
  };
  wglChoosePixelFormatARB(hdc, pf_attrs, 0, 1, &pf, &num_formats);

  // Damian: This here is taked from the rad dbg gl setup.
  //         This is very weird and win32 + gl is not great when it comes to set up.
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 32; 
  pfd.cDepthBits   = 24; 
  pfd.cStencilBits = 8;  
  pfd.iLayerType   = PFD_MAIN_PLANE; 
  // Damian: Not calling describe and choose pf, since technically we have already done that with the wgl call.
  //         But we still have to create the pfd to pass into the SetPixel call, so here it is.
  //         Its the same as from the fake window, just copy_paste.
  SetPixelFormat(hdc, pf, &pfd);

  wglMakeCurrent(hdc, gl_context);
}

void r_gl_win32_remove_window()
{
  g_win32_gl_renderer->window = 0;

  // TODO: This shoud not be done per window FOR SURE
  {
    // Unloading all the functions
    #define GL_FUNC_EXP(Type, name, parameters) name = 0;
      GL_FUNC_TABLE
    #undef GL_FUNC_EXP
  }
}

///////////////////////////////////////////////////////////
// Damian: Opengl frame
//
void DEBUG_r_gl_win32_render_rect(Rect rect);

void r_gl_win32_begin_frame()
{
  Win32_window* window = g_win32_gl_renderer->window;
  
  // Window per frame stuff
  {
    Assert(arena_is_clear(window->frame_event_arena));
    Assert(window->frame_event_list == 0);
    window->frame_event_list = ArenaPush(window->frame_event_arena, Event_list);
    win32_handle_messages(window);
  }

  // Renderer per frame stuff
  {
    Arena* frame_arena                             = g_win32_gl_renderer->frame_arena;
    F64** frame_start_time_in_sec                  = &g_win32_gl_renderer->frame_start_time_in_sec;
    Rect** viewport_rect__top_left_to_bottom_right = &g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right;
    DEBUG_draw_rect_list** draw_list               = &g_win32_gl_renderer->draw_list;

    Assert(frame_arena);
    Assert(arena_is_clear(frame_arena));
    Assert(*frame_start_time_in_sec == 0);
    Assert(*viewport_rect__top_left_to_bottom_right == 0);
    Assert(*draw_list == 0);

    *frame_start_time_in_sec                 = ArenaPush(frame_arena, F64);
    *viewport_rect__top_left_to_bottom_right = ArenaPush(frame_arena, Rect);
    *draw_list                               = ArenaPush(frame_arena, DEBUG_draw_rect_list);

    **frame_start_time_in_sec                 = os_win32_get_monotonic_time();
    **viewport_rect__top_left_to_bottom_right = win32_get_client_area_rect(window);
    **draw_list                               = DEBUG_draw_rect_list{};

    U32 width  = (U32)(**viewport_rect__top_left_to_bottom_right).width;
    U32 height = (U32)(**viewport_rect__top_left_to_bottom_right).height;
    glViewport(0, 0, width, height); 
  }
}

void r_gl_win32_end_frame()
{
  Arena* frame_arena                            = g_win32_gl_renderer->frame_arena;
  F64* frame_start_time_in_sec                  = g_win32_gl_renderer->frame_start_time_in_sec;
  Rect* viewport_rect__top_left_to_bottom_right = g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right;
  DEBUG_draw_rect_list* draw_list               = g_win32_gl_renderer->draw_list;

  Assert(frame_arena);
  Assert(!arena_is_clear(frame_arena)); 
  Assert(frame_start_time_in_sec);
  Assert(viewport_rect__top_left_to_bottom_right);
  Assert(draw_list);

  // Draw to the backbuffer
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (DEBUG_draw_rect_node* node = draw_list->first; node != 0; node = node->next) {
      if (node->is_rect) { Assert(!node->is_texture); }
      if (node->is_texture) { Assert(!node->is_rect); }
      DEV_draw_rect_list(node);
    }
  }
  
  // Enforce fps
  {
    F64 frame_time = 1.0 / r_gl_win32_get_frame_rate();
    F64 current_frame_time = os_win32_get_monotonic_time() - *frame_start_time_in_sec;
    while (current_frame_time < frame_time) {
      current_frame_time = os_win32_get_monotonic_time() - *frame_start_time_in_sec;
    }
  }
  
  // Display the frame
  SwapBuffers(g_win32_gl_renderer->window->hdc);
  
  // Clear per frame state: Window
  {
    Win32_window* window = g_win32_gl_renderer->window;
    window->frame_event_list = 0;
    arena_clear(window->frame_event_arena);
  }

  // Clear per frame state: Renderer
  {
    arena_clear(frame_arena);
    g_win32_gl_renderer->frame_start_time_in_sec = 0;
    g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right = 0;
    g_win32_gl_renderer->draw_list = 0;
  }
}

void r_gl_win32_set_frame_rate(U32 frame_rate)
{
  // TODO: Do we need to check the state here
  //       What if the user serts this befor the state is inited, 
  //       then init will set frame rate back to the default value.
  //       ???
  g_win32_gl_renderer->frame_rate = frame_rate;
}

F64 r_gl_win32_get_frame_rate()
{
  return g_win32_gl_renderer->frame_rate;
}

Texture2D load_texture(Image2D image)
{
  // TODO: Assert that the context is valid, if not then we have to warn or something
  Assert(image.data_buffer_opt.count > 0);
  Assert(image.n_chanels == 4);

  // TODO: Do we expect the image to be valid here

  GLuint texture = {};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data_buffer_opt.data);
  glBindTexture(GL_TEXTURE_2D, 0);  
  glGenerateMipmap(GL_TEXTURE_2D);

  Texture2D result = {};
  result.width = image.width;
  result.height = image.height;
  result.gl_id = texture;
  result.n_chanels = 4;

  return result;
}


///////////////////////////////////////////////////////////
// Damian: Helpers and some internal state managers
//
void* r_gl_win32_load_extension_functions_opt(const char* name)
{
  // HOTE: This is taken from the Krono "how to load gl functions". Link: https://wikis.khronos.org/opengl/Load_OpenGL_Functions
  void *p = (void *)wglGetProcAddress(name);
  if ( p == 0 
    || p == (void*)0x1 
    || (p == (void*)0x2) 
    || (p == (void*)0x3) 
    || (p == (void*)-1) 
  ) {
    p = 0;
  }
  return p;
}

void r_gl_debug_message_callback(GLenum source, GLenum type, GLuint id, 
                                 GLenum severity, GLsizei length, 
                                 const GLchar *message, const void *userParam
) {
  UnusedVar(source);
  UnusedVar(type);
  UnusedVar(id);
  UnusedVar(severity);
  UnusedVar(length);
  UnusedVar(userParam);
  printf("GL error: \n");
  printf("%s \n", message);
  printf("\n");
}

///////////////////////////////////////////////////////////
// Damian: Draw calls
//
void draw_rect(Rect rect, Color color)
{
  Arena* frame_arena = g_win32_gl_renderer->frame_arena;
  DEBUG_draw_rect_list* list = g_win32_gl_renderer->draw_list;
  
  DEBUG_draw_rect_node* node = ArenaPush(frame_arena, DEBUG_draw_rect_node);
  node->is_rect = true;
  node->rect = rect;
  node->rect_color = color;

  DllPushBack(list, node);
  list->count += 1;
}

void test_draw_texture_pro(
  Texture2D texture, 
  Rect texture_source_rect,
  Rect texture_dest_rect,
  Color color
) {
  // Crop the texture, then just draw it
  Arena* frame_arena = g_win32_gl_renderer->frame_arena;
  DEBUG_draw_rect_list* list = g_win32_gl_renderer->draw_list;
  DEBUG_draw_rect_node* node = ArenaPush(frame_arena, DEBUG_draw_rect_node);

  node->is_texture = true;
  node->texture = texture;
  node->texture_source_rect = texture_source_rect;
  node->texture_dest_rect = texture_dest_rect;
  node->rect_color = color;

  DllPushBack(list, node);
  list->count += 1;

  // draw_rect(texture_dest_rect, color);
}

void test_draw_texture_crop(
  Texture2D texture, 
  Rect source_rect, 
  F32 screen_offset_x, F32 screen_offset_y, 
  Color color
) {
  Rect dest_rect = rect_make(screen_offset_x, screen_offset_y, source_rect.width, source_rect.height);
  test_draw_texture_pro(texture, source_rect, dest_rect, color);
}

// TODO: Uncomment this back
// void test_draw_texture(Texture2D texture, F32 x, F32 y)
// {
  // Rect texture_rect = rect_make(0.0f, 0.0f, (F32)texture.width, (F32)texture.height);
  // test_draw_texture_pro(texture, texture_rect, x, y);
// }

// TODO: Move this 
#include "font/font.h"
#include "font/font.cpp"

// The texture will just be the texture
// Then i pass in the colors to blend that texture with
void test_draw_text(Font_info* font_info, Texture2D font_texture, Str8 text, F32 x, F32 y, Color color)
{
  // TODO: Rename this baseline_y to something like "top-font-line", cause its not a baseline YET.
  F32 baseline_y = y + font_info->ascent;
  F32 x_offset = x;
  
  ForEachEx(index, text.count, text.data)
  {
    U8 codepoint = text.data[index];
    
    Font_codepoint_data_node* data = font_get_codepoint_node_opt(font_info, codepoint);
    if (data)
    {
      // x_offset += data->left_side_bearing;
      Rect codepoint_rect = codepoint_rect_from_data(font_info, data);
      
      // TODO: Make sure i am not 1 px off due to range includeness
      Rect dest_rect = rect_make(x_offset + data->left_side_bearing, baseline_y - data->glyph_bbox.y1, codepoint_rect.width, codepoint_rect.height);
      test_draw_texture_pro(font_texture, codepoint_rect, dest_rect, color);
      
      x_offset += data->advance_width;
      if (index < text.count - 1)
      {
        U8 next_codepoint = text.data[index + 1];
        Font_kern_pair* kern_pair = font_get_kern_pair_opt(font_info, codepoint, next_codepoint);      
        if (kern_pair)
        {
          x_offset += kern_pair->advance;
        }
      }
    }
    else 
    {
      x_offset += font_info->max_advance_width;
    }
  }

}

void test_draw_text_lines(Font_info* font_info, Str8 text, F32 x, F32 y)
{
  F32 font_max_height = font_info->ascent - font_info->descent;
  
  // The highest point for a line of text
  {
    Rect line = rect_make(x, y, 500, 1);
    draw_rect(line, C_GREEN);
  }

  // Baseline
  {
    Rect line = rect_make(x, y + font_info->ascent, 500, 1);
    draw_rect(line, C_GREEN);
  }
  
  // The lowest point for a line of text
  {
    Rect line = rect_make(x, y + font_max_height, 500, 1);
    draw_rect(line, C_GREEN);
  }
  
  Vec2_F32 font_dims = font_measure_text(font_info, text);
  Rect complete_text_rect = rect_make(x, y, 0, font_max_height);
  complete_text_rect.width = font_dims.x;
  Assert((U32)font_max_height == (U32)font_dims.y, "Just making sure i understand the ttf here correctly.");
  draw_rect(complete_text_rect, vec4_f32(1.0f, 0.0f, 0.0f, 0.3f));
}

void DEV_draw_rect_list(DEBUG_draw_rect_node* node)
{
  B32 is_rect       = node->is_rect;
  B32 is_texture    = node->is_texture;
  Rect rect         = node->rect;
  Color rect_color  = node->rect_color;

  Texture2D texture = node->texture;
  Rect texture_source_rect = node->texture_source_rect;
  Rect texture_dest_rect = node->texture_dest_rect;
  if (is_texture) {
    rect = texture_dest_rect;
    // TODO: This has to be unionised here, 
    //       now is time (tonight is the night).
    //       Cause its getting a bit out of hand now.
  }

  Assert(is_rect_program_loaded);
  Assert(g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right);
  
  Rect vp_rect = *(g_win32_gl_renderer->viewport_rect__top_left_to_bottom_right); 

  Mat4 mat4_scale     = mat4x4_f32_scale(rect.width, rect.height, 1);
  Mat4 mat4_translate = mat4x4_f32_translate(rect.x, rect.y, 0);
  Mat4 mat4_ortho     = mat4x4_f32_ortho(0, vp_rect.width, vp_rect.height, 0, -1, 1);
  
  glBindVertexArray(rect_vao_id);    
  glUseProgram(rect_program_id);  
  
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "scale"), 1, GL_TRUE, (F32*)&mat4_scale.x);
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "projection"), 1, GL_TRUE, (F32*)&mat4_ortho.x);
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "translate"), 1, GL_TRUE, (F32*)&mat4_translate.x);

  glUniform1f(glGetUniformLocation(rect_program_id, "t_width"), (GLfloat)texture.width);
  glUniform1f(glGetUniformLocation(rect_program_id, "t_height"), (GLfloat)texture.height);

  glUniform1f(glGetUniformLocation(rect_program_id, "t_source_offset_x"), texture_source_rect.x);
  glUniform1f(glGetUniformLocation(rect_program_id, "t_source_offset_y"), texture_source_rect.y);

  glUniform1f(glGetUniformLocation(rect_program_id, "t_source_rect_width"), texture_source_rect.width);
  glUniform1f(glGetUniformLocation(rect_program_id, "t_source_rect_height"), texture_source_rect.height);

  if (is_texture)
  {
    glUniform1i(glGetUniformLocation(rect_program_id, "is_texture1"), GL_TRUE); 
    glUniform1i(glGetUniformLocation(rect_program_id, "texture1"), 0);
    glUniform4f(glGetUniformLocation(rect_program_id, "rect_color"), rect_color.r, rect_color.g, rect_color.b, rect_color.a); 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.gl_id);
  }
  else if (is_rect) 
  {
    glUniform1i(glGetUniformLocation(rect_program_id, "is_texture1"), GL_FALSE); 
    glUniform4f(glGetUniformLocation(rect_program_id, "rect_color"), rect_color.r, rect_color.g, rect_color.b, rect_color.a); 
  }
  else {
    InvalidCodePath();
  }

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // glActiveTexture(0);
  // glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
  glUseProgram(0);
}

// WIN32_LEAN_AND_MEAN

///////////////////////////////////////////////////////////
// Damian: Program loader
//
void gl_load_rect_program()
{
  Assert(!is_rect_program_loaded);
  if (is_rect_program_loaded) { return; }

  struct Vertex {
    F32 pos[2], tex[2];
  };
  
  GLuint vao = 0;
  {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
  }

  GLuint vbo = 0;
  {
    struct Vertex v_data[] = {
      // Pos          // Tex
      { {0.0f, 1.0f}, {0.0f, 1.0f}, },
      { {1.0f, 1.0f}, {1.0f, 1.0f}, },
      { {0.0f, 0.0f}, {0.0f, 0.0f}, },
      { {1.0f, 0.0f}, {1.0f, 0.0f}, },
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(v_data), v_data, GL_STATIC_DRAW);
  }

  // Filling up the vao
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)MemberOffset(Vertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)MemberOffset(Vertex, tex));
  glEnableVertexAttribArray(1);

  GLuint ebo = 0;
  {
    U32 indices[] = {
      0, 1, 2,
      1, 2, 3,
    };
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  }

  GLuint program = 0;
  {
    const char* v_shader_src[] = {
      StringLine("#version 330 core"),
      StringNewL,
      StringLine("layout (location = 0) in vec2 aPos;"),
      StringLine("layout (location = 1) in vec2 aTexturePos;"),
      StringNewL,
      StringLine("uniform mat4 projection;"),
      StringLine("uniform mat4 scale;"),
      StringLine("uniform mat4 translate;"),
      StringNewL,
      StringLine("// This is for Raylib DrawTexturePro like texture drawing"),
      StringLine("uniform float t_width;"),
      StringLine("uniform float t_height;"),
      StringLine("uniform float t_source_offset_x;"),
      StringLine("uniform float t_source_offset_y;"),
      StringLine("uniform float t_source_rect_width;"),
      StringLine("uniform float t_source_rect_height;"),
      StringNewL,
      StringLine("out vec2 TextureCoord;"),
      StringNewL,
      StringLine("void main()"),
      StringLine("{"),
      StringLine("  float t_source_offset_x_norm = t_source_offset_x / t_width;"),
      StringLine("  float t_source_offset_y_norm = t_source_offset_y / t_height;"),
      
      StringLine("  float t_source_rect_scale_x_norm = t_source_rect_width / t_height;"),
      StringLine("  float t_source_rect_scale_y_norm = t_source_rect_height / t_height;"),
      
      StringLine("  vec2 t_source_offset = vec2(t_source_offset_x_norm, t_source_offset_y_norm);"),
      StringLine("  vec2 t_source_scale_for_rect = vec2(t_source_rect_scale_x_norm, t_source_rect_scale_y_norm);"),
      StringLine("  TextureCoord = t_source_offset + t_source_scale_for_rect * aTexturePos;"),
      StringLine("  gl_Position = projection * translate *  scale * vec4(aPos.x, aPos.y, 0.0, 1.0);"),
      StringLine("}"),
    };

    const char* f_shader_src[] = {
      StringLine("#version 330 core"),
      StringNewL,
      StringLine("in vec2 TextureCoord;"),
      StringNewL,
      StringLine("uniform bool is_texture1;"),
      StringLine("uniform sampler2D texture1;"),
      // StringNewL,
      // StringLine("uniform bool is_text;"),
      // StringLine("uniform fload text_alpa;"),
      StringNewL,
      StringLine("uniform vec4 rect_color;"),
      StringNewL,
      StringLine("out vec4 FragColor;"),
      StringNewL,
      StringLine("void main()"),
      StringLine("{"),
      StringLine("  if (is_texture1)"),
      StringLine("  {"),
      StringLine("    FragColor = texture(texture1, TextureCoord);"), 
      StringLine("    FragColor = vec4(rect_color.rgb, FragColor.a);"), 
      StringLine("  }"),
      StringLine("  else"),
      StringLine("  {"),
      StringLine("    FragColor = rect_color;"),
      StringLine("  }"),
      StringLine("}"),
    };
    
    program = glCreateProgram();

    GLuint v_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint f_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(v_shader, ArrayCount(v_shader_src), (const GLchar**)&v_shader_src, Null);
    glShaderSource(f_shader, ArrayCount(f_shader_src), (const GLchar**)&f_shader_src, Null);

    glCompileShader(v_shader);
    glCompileShader(f_shader);

    {
      GLint compiled = 0;
      glGetShaderiv(v_shader, GL_COMPILE_STATUS, &compiled);
      if (!compiled)
      {
          char message[1024];
          glGetShaderInfoLog(v_shader, sizeof(message), NULL, message);
          printf("Vertex shader compilation error:\n");
          printf("%s\n", message);
          Assert(!"Failed to compile vertex shader!");
      }
    }

    {
      GLint compiled = 0;
      glGetShaderiv(f_shader, GL_COMPILE_STATUS, &compiled);
      if (!compiled)
      {
          char message[1024];
          glGetShaderInfoLog(f_shader, sizeof(message), NULL, message);
          printf("Fragment shader compilation error:\n");
          printf("%s\n", message);
          Assert(!"Failed to compile fragment shader!");
      }
    }

    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);
    {
      GLint linked = 0;
      glGetProgramiv(program, GL_LINK_STATUS, &linked);
      if (!linked)
      {
          char message[1024];
          glGetProgramInfoLog(v_shader, sizeof(message), NULL, message);
          printf("Program link error: \n");
          printf("%s \n", message);
          //OutputDebugStringA(message);
          // Assert(!"Failed to create vertex shader!");
      }
    }
  }

  // Damian: Have to unbind the vao first.
  //         If ebo or vbo unbound before the vao, then the vao state changes, --> UB.
  glBindVertexArray(Null);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Null);

  is_rect_program_loaded = true;
  rect_vbo_id            = vbo;
  rect_ebo_id            = ebo;
  rect_vao_id            = vao; 
  rect_program_id        = program;
}












#endif

