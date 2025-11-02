#ifndef RENDER_GL_WIN32_CPP
#define RENDER_GL_WIN32_CPP

#include "render/render_gl_win32.h"
#include "os/core/os_core_win32.h" // TODO: See maybe can pull this into the header file instead of having it here

#include "other/image_stuff/image_loader.h"

///////////////////////////////////////////////////////////
// Damian: Opengl state globals
//
GL_renderer g_win32_gl_renderer = {};

#define R_WIN32_GL_DEFAULT_FRAME_RATE ((U32)60)
#define R_WIN32_GL_DEFAULT_IS_HEIGHT_BOTTOM_UP false

///////////////////////////////////////////////////////////
// Damian: Opengl specific helpers for state managment
//
void DEBUG_load_rect();
void DEBUG_unload_rect();
static B32 is_rect_loaded = false;
static GLuint rect_vbo_id = 0;
static GLuint rect_ebo_id = 0;
static GLuint rect_vao_id = 0; 
static GLuint rect_program_id = 0;

void DEBUG_load_rect()
{
  Assert(g_win32_gl_renderer.context);
  if (is_rect_loaded) { return; }
  
  // Creating a buffer for the draw call
  GLfloat rect_vertices[] = {
    // Pos        // T map
    0.0f, 1.0f,   0.0f, 1.0f, // Top left
    1.0f, 1.0f,   1.0f, 1.0f, // Top right
    0.0f, 0.0f,   0.0f, 0.0f, // Bottom left
    1.0f, 0.0f,   1.0f, 0.0f, // Bottom right
  };

  GLuint rect_indices[] = {
    0, 1, 2, 
    1, 2, 3
  };

  // NOTE: This has to be created first to capture the buffer state for vbo and ebo
  GLuint vao_id;
  glGenVertexArrays(1, &vao_id);
  glBindVertexArray(vao_id);
  
  GLuint vbo_id;
  glGenBuffers(1, &vbo_id);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect_vertices), rect_vertices, GL_STATIC_DRAW);

  GLuint ebo_id;
  glGenBuffers(1, &ebo_id);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rect_indices), rect_indices, GL_STATIC_DRAW);

  // NOTE: This can only be called when the vao and the buffers with data for it are alredy set
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // Creating a program to draw
  GLuint v_shader_id = 0;
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
      StringLine("out vec2 TextureCoord;"),
      StringNewL,
      StringLine("void main()"),
      StringLine("{"),
      StringLine("  TextureCoord = aTexturePos;"),
      StringLine("  gl_Position = projection * translate *  scale * vec4(aPos.x, aPos.y, 0.0, 1.0);"),
      StringLine("}"),
    };

    v_shader_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader_id, ArrayCount(v_shader_src), (const GLchar**)&v_shader_src, Null);
    glCompileShader(v_shader_id);
    {
      GLint success = 0;
      glGetShaderiv(v_shader_id, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        printf("ERROR: Vertex shader compilation failed!\n");
      }

      GLint info_log_len = 0;
      glGetShaderiv(v_shader_id, GL_INFO_LOG_LENGTH, &info_log_len);
      if (info_log_len != 0)
      {
        Assert(info_log_len > 0);
        Scratch scratch = get_scratch();
        Data_buffer buffer = {};
        buffer.count = (U64)info_log_len;
        buffer.data = ArenaPushArr(scratch.arena, U8, buffer.count); 
        glGetShaderInfoLog(v_shader_id, buffer.count, Null, (char*)buffer.data);
        {
          Str8 log = str8_from_str8_temp_null_term(scratch.arena, buffer);
          printf("Vertex shader info log: \n");
          printf("%s", log.data);
          printf("\n");
        }
        end_scratch(&scratch);
      }
    }
  }
    
  GLuint f_shader_id = 0;
  {
    const char* f_shader_src[] = {
      StringLine("#version 330 core"),
      StringNewL,
      StringLine("in vec2 TextureCoord;"),
      StringNewL,
      StringLine("uniform sampler2D texture1;"),
      StringNewL,
      StringLine("out vec4 FragColor;") 
      StringNewL,
      StringLine("void main()"),
      StringLine("{"),
      StringLine("  FragColor = texture(texture1, TextureCoord);"),  
      StringLine("}"),
    } ;

    
    f_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader_id, ArrayCount(f_shader_src), (const GLchar**)&f_shader_src, Null);
    glCompileShader(f_shader_id);
    {
      GLint success = 0;
      glGetShaderiv(f_shader_id, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        printf("ERROR: Fragment shader compilation failed!\n");
      }

      GLint info_log_len = 0;
      glGetShaderiv(f_shader_id, GL_INFO_LOG_LENGTH, &info_log_len);
      if (info_log_len != 0)
      {
        Assert(info_log_len > 0);
        Scratch scratch = get_scratch();
        Data_buffer buffer = {};
        buffer.count = (U64)info_log_len;
        buffer.data = ArenaPushArr(scratch.arena, U8, buffer.count); 
        glGetShaderInfoLog(f_shader_id, buffer.count, Null, (char*)buffer.data);
        {
          Str8 log = str8_from_str8_temp_null_term(scratch.arena, buffer);
          printf("Fragment shader info log: \n");
          printf("%s", log.data);
          printf("\n");
        }
        end_scratch(&scratch);
      }
    }
  }

  GLuint program_id = glCreateProgram();
  glAttachShader(program_id, v_shader_id);
  glAttachShader(program_id, f_shader_id);
  glLinkProgram(program_id);
  {
    GLint success = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success)
    {
      printf("ERROR: Program linking failed!\n");
    }

    GLint info_log_len = 0;
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len != 0) 
    {
      Assert(info_log_len > 0);
      Scratch scratch = get_scratch();
      Data_buffer buffer = {};
      buffer.count = (U64)info_log_len;
      buffer.data = ArenaPushArr(scratch.arena, U8, buffer.count); 
      glGetProgramInfoLog(program_id, buffer.count, Null, (char*)buffer.data);
      {
        Str8 log = str8_from_str8_temp_null_term(scratch.arena, buffer);
        printf("Program link info log: \n");
        printf("%s", log.data);
        printf("\n");
      }
      end_scratch(&scratch);
    }
  }

  is_rect_loaded = true;
  rect_vbo_id = vbo_id;
  rect_ebo_id = ebo_id;
  rect_vao_id = vao_id;
  rect_program_id = program_id;

  // Cleaning
  // glBindBuffer(GL_ARRAY_BUFFER, 0);         // -> These two might stay this way
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // -> 
  
  glBindVertexArray(0);
  glUseProgram(0);
}
void DEBUG_unload_rect()
{
  is_rect_loaded = false;
  
  // TODO: More formal gl cleanup
  rect_vbo_id = 0;
  rect_ebo_id = 0;
  rect_vao_id = 0; 
  rect_program_id = 0;
}

///////////////////////////////////////////////////////////
// Damian: Opengl renderer managing functions
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

///////////////////////////////////////////////////////////
// Damian: Opengl state init
//
void r_gl_win32_init() 
{
  g_win32_gl_renderer = GL_renderer{};
  
  // Making a fake window and context to load gl functions
  WNDCLASSEXW fake_window_class = {};
  HWND fake_window_handle = {};
  HDC fake_hdc = {};
  HGLRC fake_gl_context = {};
  { 
    fake_window_class.cbSize        = sizeof(WNDCLASSEXW); 
    fake_window_class.style         = CS_HREDRAW | CS_VREDRAW; 
    fake_window_class.lpfnWndProc   = DefWindowProc; 
    fake_window_class.hInstance     = GetModuleHandleW(Null); 
    fake_window_class.hIcon         = LoadIcon(NULL, IDI_WINLOGO); 
    fake_window_class.hCursor       = LoadCursor(NULL, IDC_ARROW); 
    fake_window_class.lpszClassName = L"Fake window for gl loading"; 
    RegisterClassExW(&fake_window_class);
    
    fake_window_handle = CreateWindowExW(
      0L,
      fake_window_class.lpszClassName,
      L"Fake window",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      500,
      500,
      Null,
      Null,
      fake_window_class.hInstance,
      Null);
    ShowWindow(fake_window_handle, SW_HIDE);
      
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = 32;
    pfd.cDepthBits   = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType   = PFD_MAIN_PLANE;
  
    fake_hdc = GetDC(fake_window_handle);
    int pf = ChoosePixelFormat(fake_hdc, &pfd);
    SetPixelFormat(fake_hdc, pf, &pfd);
    fake_gl_context = wglCreateContext(fake_hdc);
    wglMakeCurrent(fake_hdc, fake_gl_context); // TODO: Do i need to assert here or just check the succ
  }

  
  // Loading newer gl funcs
  {
    #define GL_FUNC_EXP(Type, name, parameters) name = (name##_FuncType*)r_gl_win32_load_extension_functions_opt(#name);
      GL_FUNC_TABLE
    #undef GL_FUNC_EXP
    
    #define GL_FUNC_EXP(Type, name, parameters) Assert(name);
      GL_FUNC_TABLE
    #undef GL_FUNC_EXP
  }
  
  // Removing the fake window and context
  wglMakeCurrent(fake_hdc, 0); // NOTE: After this call, glGetError return 1282, its fine, since there is not active context for glError to work
  wglDeleteContext(fake_gl_context);
  ReleaseDC(fake_window_handle, fake_hdc);
  DestroyWindow(fake_window_handle);
  UnregisterClassW(fake_window_class.lpszClassName, Null);
  
  // Preparing all the things i will need for the entire renderer lifetime
  // DEBUG_load_rect(); // NOTE: This can only be dont when gl context is present
  g_win32_gl_renderer.frame_arena = arena_alloc(Kilobytes_U64(10), "OpenGL frame arena");
  g_win32_gl_renderer.frame_rate = R_WIN32_GL_DEFAULT_FRAME_RATE;
  g_win32_gl_renderer.is_height_bottom_up = R_WIN32_GL_DEFAULT_IS_HEIGHT_BOTTOM_UP;
  g_win32_gl_renderer.perm_arena = arena_alloc(Megabytes_U64(10), "Opengl perm(lifetime) arena");
}

void r_gl_win32_end()
{
  // Unloading all the functions
  #define GL_FUNC_EXP(Type, name, parameters) name = 0;
    GL_FUNC_TABLE
  #undef GL_FUNC_EXP

  // Release all the opengl specific state that was created for the enitire rendered lifetime 
  DEBUG_unload_rect();
  arena_release(g_win32_gl_renderer.frame_arena);
  FreeLibrary(g_win32_gl_renderer.opengl32_dll_module);

  g_win32_gl_renderer = GL_renderer{};
}

///////////////////////////////////////////////////////////
// Damian: Opengl window
//
void r_gl_win32_equip_window(Win32_window* window)
{
  Assert(window);

  g_win32_gl_renderer.window = window;

  // NOTE: Taken from the official Krono set up page. Link: https://wikis.khronos.org/opengl/Creating_an_OpenGL_Context_(WGL)
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion     = 1;
  pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType   = PFD_TYPE_RGBA;
  pfd.cColorBits   = 32;
  pfd.cDepthBits   = 24;
  pfd.cStencilBits = 8;
  pfd.iLayerType   = PFD_MAIN_PLANE;

  int pf = ChoosePixelFormat(window->hdc, &pfd);
  SetPixelFormat(window->hdc, pf, &pfd);
  g_win32_gl_renderer.context = wglCreateContext(window->hdc);
  wglMakeCurrent(window->hdc, g_win32_gl_renderer.context); // TODO: Do i need to assert here or just check the succ
  
  DEBUG_load_rect();
}

void r_gl_win32_remove_window()
{
  g_win32_gl_renderer.window = 0;
}

///////////////////////////////////////////////////////////
// Damian: Opengl frame
//
void DEBUG_r_gl_win32_render_rect(Rect rect);

void r_gl_win32_begin_frame()
{
  Win32_window* window = g_win32_gl_renderer.window;
  
  // Window per frame stuff
  {
    Assert(arena_is_clear(window->frame_event_arena));
    Assert(window->frame_event_list == 0);
    window->frame_event_list = ArenaPush(window->frame_event_arena, Event_list);
    win32_handle_messages(window);
  }

  // Renderer per frame stuff
  {
    Arena* frame_arena = g_win32_gl_renderer.frame_arena;
    Frame_data** frame_data = &g_win32_gl_renderer.frame_data;
    Assert(arena_is_clear(frame_arena));
    Assert(*frame_data == 0);
    *frame_data = ArenaPush(frame_arena, Frame_data);
    // arena_clear(g_win32_gl_renderer.frame_arena);
    
    Rect frame_rect = win32_get_client_area_rect(g_win32_gl_renderer.window);
    (*frame_data)->viewport_rect__top_left_top_to_bottom = frame_rect; 
    glViewport(0, 0, frame_rect.width, frame_rect.height); 
    g_win32_gl_renderer.frame_start_time_in_sec = get_monotonic_time();
  }
}

void r_gl_win32_end_frame()
{
  // Draw to the backbuffer
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Frame_data* frame_data = g_win32_gl_renderer.frame_data;
    DEBUG_draw_rect_list* draw_list = &frame_data->draw_list;
    for (DEBUG_draw_rect_node* node = draw_list->first; node != 0; node = node->next) {
      DEBUG_r_gl_win32_render_rect(node->rect);
    }
  }
  
  // Enforce fps
  {
    F64 frame_time = 1.0 / g_win32_gl_renderer.frame_rate;
    F64 current_frame_time = get_monotonic_time() - g_win32_gl_renderer.frame_start_time_in_sec;
    while (current_frame_time < frame_time) {
      current_frame_time = get_monotonic_time() - g_win32_gl_renderer.frame_start_time_in_sec;
    }
  }
  
  // Display the frame
  SwapBuffers(g_win32_gl_renderer.window->hdc);
  
  // Clear per frame state: Window
  {
    Win32_window* window = g_win32_gl_renderer.window;
    window->frame_event_list = 0;
    arena_clear(window->frame_event_arena);
  }

  // Clear per frame state: Renderer
  {
    arena_clear(g_win32_gl_renderer.frame_arena);
    g_win32_gl_renderer.frame_data = 0;
  }
}

void r_gl_win32_set_frame_rate(U32 frame_rate)
{
  // TODO: Do we need to check the state here
  //       What if the user serts this befor the state is inited, 
  //       then init will set frame rate back to the default value.
  //       ???
  g_win32_gl_renderer.frame_rate = frame_rate;
}

F64 r_gl_win32_get_frame_rate()
{
  return g_win32_gl_renderer.frame_rate;
}

///////////////////////////////////////////////////////////
// Damian: TESTING THIS STUFF
//
void r_gl_win32_set_screen_origin_top_left()
{
  g_win32_gl_renderer.is_height_bottom_up = false;
}

void r_gl_win32_set_screen_origin_bottom_left()
{
  g_win32_gl_renderer.is_height_bottom_up = true;
}

// TODO: This shoud be removed from here later (November 1st (Saturday) 2025, 13:19)
///////////////////////////////////////////////////////////
// Damian: DEBUG Opengl draw
//
void draw_rect(Rect rect)
{
  Arena* frame_arena = g_win32_gl_renderer.frame_arena;
  DEBUG_draw_rect_list* list = &g_win32_gl_renderer.frame_data->draw_list;
  DEBUG_draw_rect_node* node = ArenaPush(frame_arena, DEBUG_draw_rect_node);
  node->rect = rect;
  DllPushBack(list, node);
  list->count += 1;
}

void DEBUG_r_gl_win32_render_rect(Rect rect)
{
  // TODO: See that to do with this thing here, assert or nah?
  //       Probably just a check, then load and warning to a log file or something
  // Assert(is_rect_loaded); 
  DEBUG_load_rect(); 

  // TODO: Height has to be flipped here in a nice way
  Frame_data* frame_data = g_win32_gl_renderer.frame_data;
  Assert(frame_data != 0);
  Rect viewport_rect = frame_data->viewport_rect__top_left_top_to_bottom;
  
  Mat4 mat4_scale = mat4x4_f32_scale(rect.width, rect.height, 1);
  Mat4 mat4_translate = mat4x4_f32_translate(rect.x, rect.y, 0);
  Mat4 mat4_ortho = mat4x4_f32_identity();
  if (g_win32_gl_renderer.is_height_bottom_up) {
    mat4_ortho = mat4x4_f32_ortho(0, viewport_rect.width, 0, viewport_rect.height, -1, 1);
  } else {
    mat4_ortho = mat4x4_f32_ortho(0, viewport_rect.width, viewport_rect.height, 0, -1, 1);
  }
  
  glBindVertexArray(rect_vao_id);    
  glUseProgram(rect_program_id);  
  
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "scale"), 1, GL_TRUE, (F32*)&mat4_scale.x);
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "projection"), 1, GL_TRUE, (F32*)&mat4_ortho.x);
  glUniformMatrix4fv(glGetUniformLocation(rect_program_id, "translate"), 1, GL_TRUE, (F32*)&mat4_translate.x);

  // Load a texture here
  static GLuint texture = 0;
  static B32 texture_loaded = false;
  if (!texture_loaded)
  {
    texture_loaded = true;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    {
      unsigned char* data = 0;
      int width = 0, height = 0, nrChannels = 0;
      {
        stbi_set_flip_vertically_on_load(true);
        data = stbi_load("../data/jimmy.png", &width, &height, &nrChannels, 0);
        int x = 0;
      }
      if (data)
      {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      else 
      {
        const char* error_message = stbi_failure_reason();
        fprintf(stderr, "Error loading image: %s\n", error_message);
      }
      stbi_image_free(data);

      Arena* arena = g_win32_gl_renderer.perm_arena;
      B32 do_flip_y = g_win32_gl_renderer.is_height_bottom_up;
      Image_2d image = load_png(arena, Str8FromClit(arena, "F:/my_code/code_garbage/screen_stuff/data/test.png"), do_flip_y);
      // Assert(image.n_chanels == 3 || image.n_chanels == 4);
      // if (image.data_buffer_opt.count > 0)
      // {
      //   GLuint gl_image_chanels = GL_RGB;
      //   if (image.n_chanels == 4) {
      //     gl_image_chanels = GL_RGBA;
      //   }
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data_buffer_opt.data);
      // glGenerateMipmap(GL_TEXTURE_2D);
      // }
    }
  }

  // Binding the texture to use for gl
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(rect_program_id, "texture1"), 0); 
  }
  
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  glUseProgram(0);
}

















#endif

