#ifndef RENDER_GL_WIN32_H
#define RENDER_GL_WIN32_H

#include <windows.h>
#pragma comment(lib, "Opengl32.lib")
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

struct DEBUG_gl_texture_2d {

};

// ===================================

struct Frame_data {
  Rect viewport_rect__top_left_top_to_bottom;
  DEBUG_draw_rect_list draw_list;
};

struct GL_renderer {
  Win32_window* window;
  HGLRC context;

  // Some settings
  B32 is_height_bottom_up;

  // Persistent frame stuff
  U32 frame_rate;       

  // Per frame stuff 
  Arena* frame_arena;          
  Frame_data* frame_data;      
  F64 frame_start_time_in_sec;
};

extern GL_renderer g_win32_gl_renderer;

void* r_gl_win32_load_extension_functions_opt(const char* name);

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

#if defined(_WIN64)
  typedef signed   long long int khronos_ssize_t;
#else
  typedef signed   long  int     khronos_ssize_t;
#endif

typedef char GLchar;
typedef khronos_ssize_t GLsizeiptr;

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
  GL_FUNC_EXP(void, glBindVertexArray, (GLuint buffer)) 

#define GL_FUNC_EXP(Type, name, parameters) typedef Type (name##_FuncType) parameters;
  GL_FUNC_TABLE;
#undef GL_FUNC_EXP

#define GL_FUNC_EXP(Type, name, parameters) global name##_FuncType* name = 0;
  GL_FUNC_TABLE
#undef GL_FUNC_EXP


/* NOTE: This is the opengl 3.3 defines and function types. Link: https://registry.khronos.org/OpenGL/api/GL/glext.h
#ifndef GL_VERSION_3_3
#define GL_VERSION_3_3 1
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR    0x88FE
#define GL_SRC1_COLOR                     0x88F9
#define GL_ONE_MINUS_SRC1_COLOR           0x88FA
#define GL_ONE_MINUS_SRC1_ALPHA           0x88FB
#define GL_MAX_DUAL_SOURCE_DRAW_BUFFERS   0x88FC
#define GL_ANY_SAMPLES_PASSED             0x8C2F
#define GL_SAMPLER_BINDING                0x8919
#define GL_RGB10_A2UI                     0x906F
#define GL_TEXTURE_SWIZZLE_R              0x8E42
#define GL_TEXTURE_SWIZZLE_G              0x8E43
#define GL_TEXTURE_SWIZZLE_B              0x8E44
#define GL_TEXTURE_SWIZZLE_A              0x8E45
#define GL_TEXTURE_SWIZZLE_RGBA           0x8E46
#define GL_TIME_ELAPSED                   0x88BF
#define GL_TIMESTAMP                      0x8E28
#define GL_INT_2_10_10_10_REV             0x8D9F
typedef void (APIENTRYP PFNGLBINDFRAGDATALOCATIONINDEXEDPROC) (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETFRAGDATAINDEXPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGENSAMPLERSPROC) (GLsizei count, GLuint *samplers);
typedef void (APIENTRYP PFNGLDELETESAMPLERSPROC) (GLsizei count, const GLuint *samplers);
typedef GLboolean (APIENTRYP PFNGLISSAMPLERPROC) (GLuint sampler);
typedef void (APIENTRYP PFNGLBINDSAMPLERPROC) (GLuint unit, GLuint sampler);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIPROC) (GLuint sampler, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFPROC) (GLuint sampler, GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, const GLfloat *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, const GLint *param);
typedef void (APIENTRYP PFNGLSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, const GLuint *param);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIIVPROC) (GLuint sampler, GLenum pname, GLint *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERFVPROC) (GLuint sampler, GLenum pname, GLfloat *params);
typedef void (APIENTRYP PFNGLGETSAMPLERPARAMETERIUIVPROC) (GLuint sampler, GLenum pname, GLuint *params);
typedef void (APIENTRYP PFNGLQUERYCOUNTERPROC) (GLuint id, GLenum target);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTI64VPROC) (GLuint id, GLenum pname, GLint64 *params);
typedef void (APIENTRYP PFNGLGETQUERYOBJECTUI64VPROC) (GLuint id, GLenum pname, GLuint64 *params);
typedef void (APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP1UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP2UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP3UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIPROC) (GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXATTRIBP4UIVPROC) (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXP2UIPROC) (GLenum type, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXP2UIVPROC) (GLenum type, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXP3UIPROC) (GLenum type, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXP3UIVPROC) (GLenum type, const GLuint *value);
typedef void (APIENTRYP PFNGLVERTEXP4UIPROC) (GLenum type, GLuint value);
typedef void (APIENTRYP PFNGLVERTEXP4UIVPROC) (GLenum type, const GLuint *value);
typedef void (APIENTRYP PFNGLTEXCOORDP1UIPROC) (GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLTEXCOORDP1UIVPROC) (GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLTEXCOORDP2UIPROC) (GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLTEXCOORDP2UIVPROC) (GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLTEXCOORDP3UIPROC) (GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLTEXCOORDP3UIVPROC) (GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLTEXCOORDP4UIPROC) (GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLTEXCOORDP4UIVPROC) (GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIPROC) (GLenum texture, GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP1UIVPROC) (GLenum texture, GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIPROC) (GLenum texture, GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP2UIVPROC) (GLenum texture, GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIPROC) (GLenum texture, GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP3UIVPROC) (GLenum texture, GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIPROC) (GLenum texture, GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLMULTITEXCOORDP4UIVPROC) (GLenum texture, GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLNORMALP3UIPROC) (GLenum type, GLuint coords);
typedef void (APIENTRYP PFNGLNORMALP3UIVPROC) (GLenum type, const GLuint *coords);
typedef void (APIENTRYP PFNGLCOLORP3UIPROC) (GLenum type, GLuint color);
typedef void (APIENTRYP PFNGLCOLORP3UIVPROC) (GLenum type, const GLuint *color);
typedef void (APIENTRYP PFNGLCOLORP4UIPROC) (GLenum type, GLuint color);
typedef void (APIENTRYP PFNGLCOLORP4UIVPROC) (GLenum type, const GLuint *color);
typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIPROC) (GLenum type, GLuint color);
typedef void (APIENTRYP PFNGLSECONDARYCOLORP3UIVPROC) (GLenum type, const GLuint *color);
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glBindFragDataLocationIndexed (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
GLAPI GLint APIENTRY glGetFragDataIndex (GLuint program, const GLchar *name);
GLAPI void APIENTRY glGenSamplers (GLsizei count, GLuint *samplers);
GLAPI void APIENTRY glDeleteSamplers (GLsizei count, const GLuint *samplers);
GLAPI GLboolean APIENTRY glIsSampler (GLuint sampler);
GLAPI void APIENTRY glBindSampler (GLuint unit, GLuint sampler);
GLAPI void APIENTRY glSamplerParameteri (GLuint sampler, GLenum pname, GLint param);
GLAPI void APIENTRY glSamplerParameteriv (GLuint sampler, GLenum pname, const GLint *param);
GLAPI void APIENTRY glSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param);
GLAPI void APIENTRY glSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat *param);
GLAPI void APIENTRY glSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint *param);
GLAPI void APIENTRY glSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint *param);
GLAPI void APIENTRY glGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint *params);
GLAPI void APIENTRY glQueryCounter (GLuint id, GLenum target);
GLAPI void APIENTRY glGetQueryObjecti64v (GLuint id, GLenum pname, GLint64 *params);
GLAPI void APIENTRY glGetQueryObjectui64v (GLuint id, GLenum pname, GLuint64 *params);
GLAPI void APIENTRY glVertexAttribDivisor (GLuint index, GLuint divisor);
GLAPI void APIENTRY glVertexAttribP1ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI void APIENTRY glVertexAttribP1uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI void APIENTRY glVertexAttribP2ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI void APIENTRY glVertexAttribP2uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI void APIENTRY glVertexAttribP3ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI void APIENTRY glVertexAttribP3uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI void APIENTRY glVertexAttribP4ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
GLAPI void APIENTRY glVertexAttribP4uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
GLAPI void APIENTRY glVertexP2ui (GLenum type, GLuint value);
GLAPI void APIENTRY glVertexP2uiv (GLenum type, const GLuint *value);
GLAPI void APIENTRY glVertexP3ui (GLenum type, GLuint value);
GLAPI void APIENTRY glVertexP3uiv (GLenum type, const GLuint *value);
GLAPI void APIENTRY glVertexP4ui (GLenum type, GLuint value);
GLAPI void APIENTRY glVertexP4uiv (GLenum type, const GLuint *value);
GLAPI void APIENTRY glTexCoordP1ui (GLenum type, GLuint coords);
GLAPI void APIENTRY glTexCoordP1uiv (GLenum type, const GLuint *coords);
GLAPI void APIENTRY glTexCoordP2ui (GLenum type, GLuint coords);
GLAPI void APIENTRY glTexCoordP2uiv (GLenum type, const GLuint *coords);
GLAPI void APIENTRY glTexCoordP3ui (GLenum type, GLuint coords);
GLAPI void APIENTRY glTexCoordP3uiv (GLenum type, const GLuint *coords);
GLAPI void APIENTRY glTexCoordP4ui (GLenum type, GLuint coords);
GLAPI void APIENTRY glTexCoordP4uiv (GLenum type, const GLuint *coords);
GLAPI void APIENTRY glMultiTexCoordP1ui (GLenum texture, GLenum type, GLuint coords);
GLAPI void APIENTRY glMultiTexCoordP1uiv (GLenum texture, GLenum type, const GLuint *coords);
GLAPI void APIENTRY glMultiTexCoordP2ui (GLenum texture, GLenum type, GLuint coords);
GLAPI void APIENTRY glMultiTexCoordP2uiv (GLenum texture, GLenum type, const GLuint *coords);
GLAPI void APIENTRY glMultiTexCoordP3ui (GLenum texture, GLenum type, GLuint coords);
GLAPI void APIENTRY glMultiTexCoordP3uiv (GLenum texture, GLenum type, const GLuint *coords);
GLAPI void APIENTRY glMultiTexCoordP4ui (GLenum texture, GLenum type, GLuint coords);
GLAPI void APIENTRY glMultiTexCoordP4uiv (GLenum texture, GLenum type, const GLuint *coords);
GLAPI void APIENTRY glNormalP3ui (GLenum type, GLuint coords);
GLAPI void APIENTRY glNormalP3uiv (GLenum type, const GLuint *coords);
GLAPI void APIENTRY glColorP3ui (GLenum type, GLuint color);
GLAPI void APIENTRY glColorP3uiv (GLenum type, const GLuint *color);
GLAPI void APIENTRY glColorP4ui (GLenum type, GLuint color);
GLAPI void APIENTRY glColorP4uiv (GLenum type, const GLuint *color);
GLAPI void APIENTRY glSecondaryColorP3ui (GLenum type, GLuint color);
GLAPI void APIENTRY glSecondaryColorP3uiv (GLenum type, const GLuint *color);
#endif // GL_VERSION_3_3 
*/










#endif











