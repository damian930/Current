#ifndef RENDER_GL_WIN32_H
#define RENDER_GL_WIN32_H

#include <windows.h>
#pragma comment(lib, "opengl32.lib") // This load the old (OpenGL 1.1) functions 
#include <GL/gl.h>

#include "base/core.h"
#include "os/gfx/os_gfx_win32.h"

// TODO: These might not be good here
struct DEBUG_draw_rect_node {
  DEBUG_draw_rect_node* prev;
  DEBUG_draw_rect_node* next;
  Rect rect;
};

struct DEBUG_draw_rect_list {
  DEBUG_draw_rect_node* first;
  DEBUG_draw_rect_node* last;
  U64 count;
};

// ===================================

struct Frame_data {
  Rect viewport_rect__top_left_top_to_bottom;
  DEBUG_draw_rect_list draw_list;
};

struct GL_renderer {
  HMODULE opengl32_dll_module;

  Win32_window* window;
  HGLRC context;

  // Some settings
  B32 is_height_bottom_up;

  Arena* perm_arena;

  // Persistent frame stuff
  U32 frame_rate;       

  // Per frame stuff 
  Arena* frame_arena;          
  Frame_data* frame_data;      
  F64 frame_start_time_in_sec;
};

extern GL_renderer g_win32_gl_renderer;

void* r_gl_win32_load_extension_functions_opt(const char* name);
void* r_gl_win32_load_normal_gl_functions_opt(const char* name);

void r_gl_win32_init();
void r_gl_win32_end();

void r_gl_win32_equip_window(Win32_window* window);
void r_gl_win32_remove_window();

void r_gl_win32_begin_frame();
void r_gl_win32_end_frame();

void r_gl_win32_set_frame_rate(U32 frame_rate);
F64 r_gl_wi32_get_frame_rate();

///////////////////////////////////////////////////////////
// Damian: OS generic stuff for opengl, these are a part of OpenGL standard
//
// NOTE: Link: https://registry.khronos.org/OpenGL/api/GL/glext.h
#define GL_ARRAY_BUFFER         0x8892
#define GL_STATIC_DRAW          0x88E4
#define GL_VERTEX_SHADER        0x8B31
#define GL_FRAGMENT_SHADER      0x8B30
#define GL_INFO_LOG_LENGTH      0x8B84
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_COMPILE_STATUS       0x8B81
#define GL_LINK_STATUS          0x8B82

#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF

#if defined(_WIN64)
  typedef signed   long long int khronos_ssize_t;
#else
  typedef signed   long  int     khronos_ssize_t;
#endif

typedef char GLchar;
typedef khronos_ssize_t GLsizeiptr;

// NOTE: These are to be loaded from the wgl call
#define GL_FUNC_TABLE \
  GL_FUNC_EXP(void, glUseProgram, (GLuint program)) \
  GL_FUNC_EXP(GLuint, glCreateProgram, (void)) \
  GL_FUNC_EXP(GLuint, glCreateShader, (GLenum shaderType)) \
  GL_FUNC_EXP(void, glShaderSource, (GLuint shader, GLsizei count, const GLchar** string, const GLint *length)) \
  GL_FUNC_EXP(void, glCompileShader, (GLuint shader)) \
  GL_FUNC_EXP(void, glAttachShader, (GLuint program, GLuint shader)) \
  GL_FUNC_EXP(void, glLinkProgram, (GLuint program)) \
  GL_FUNC_EXP(void, glGetShaderiv, (GLuint shader, GLenum pname, GLint* params)) \
  GL_FUNC_EXP(void, glGetShaderInfoLog, (GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog)) \
  GL_FUNC_EXP(GLuint, glGetUniformLocation, (GLuint	program, const GLchar* name)) \
  GL_FUNC_EXP(void, glUniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat*	value)) \
  GL_FUNC_EXP(void, glGetProgramiv, (GLint program, GLenum pname, GLint* params)) \
  GL_FUNC_EXP(void, glGetProgramInfoLog, (GLuint program, GLsizei maxLength, GLsizei* length, GLchar* infoLog)) \
  \
  GL_FUNC_EXP(void, glGenBuffers, (GLsizei n, GLuint* buffers)) \
  GL_FUNC_EXP(void, glBindBuffer, (GLenum target, GLuint buffer)) \
  GL_FUNC_EXP(void, glBufferData, (GLenum	target, GLsizeiptr size, const GLvoid* data, GLenum	usage)) \
  GL_FUNC_EXP(void, glVertexAttribPointer, (GLuint index, GLint size, GLenum type, GLboolean	normalized, GLsizei	stride, const GLvoid*	pointer)) \
  GL_FUNC_EXP(void, glEnableVertexAttribArray, (GLuint index)) \
  GL_FUNC_EXP(void, glGenVertexArrays, (GLuint n, GLuint* arrays)) \
  GL_FUNC_EXP(void, glBindVertexArray, (GLuint buffer)) \
  \
  GL_FUNC_EXP(void, glActiveTexture, (GLenum texture)) \
  GL_FUNC_EXP(void, glUniform1i, (GLint location, GLint v0)) \
  GL_FUNC_EXP(void, glUniform1f, (GLint location, GLfloat v0)) \
  GL_FUNC_EXP(void, glGenerateMipmap, (GLenum target)) 

#define GL_FUNC_EXP(Type, name, parameters) typedef Type (name##_FuncType) parameters;
  GL_FUNC_TABLE;
#undef GL_FUNC_EXP

#define GL_FUNC_EXP(Type, name, parameters) global name##_FuncType* name = 0;
  GL_FUNC_TABLE
#undef GL_FUNC_EXP








#endif











