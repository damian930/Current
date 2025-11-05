#ifndef MATH_H
#define MATH_H

#include "base/core.h"

///////////////////////////////////////////////////////////
// Damian: Some regular macros
//
#define Max(x, y) (x > y ? x : y)
#define Min(x, y) (x < y ? x : y)

#define ValueToMin(value_to_min, possible_min) \
  do { if (value_to_min > possible_min) { value_to_min = possible_min; } } while(false);
#define ValueToMax(value_to_max, possible_max) \
  do { if (value_to_max > possible_max) { value_to_max = possible_max; } } while(false);

#define ValueToMinWithAction(value, possible_min, callback) \
  do { if (value > possible_min) { value = possible_min; callback; } } while(false);

#define Abs(Type, x) ( ((x) > Type{} ? (x) : (-(x))) )
#define Square(x) ((x) * (x))

struct Vec2_F32 {
  F32 x;
  F32 y;
};

struct Vec2_S32 {
  S32 x;
  S32 y;
};

struct Vec2_U32 {
  U32 x;
  U32 y;
};

union Vec3_F32 {
  struct {
    F32 x, y, z;
  };

  struct {
    F32 r, g, b;
  };
};

union Vec4_F32 {
  struct {
    F32 x, y, z, w;   
  };

  struct {
    F32 r, g, b, a;
  };
};

typedef Vec2_F32 Vec2;
typedef Vec3_F32 Vec3;
typedef Vec4_F32 Vec4;
typedef Vec4_F32 Color;

struct Rect {
  F32 x;
  F32 y;
  F32 width;
  F32 height; 
};

///////////////////////////////////////////////////////////
// Damian: Vec2_F32 stuff
//
Vec2_F32 vec2_f32(F32 x, F32 y);
Vec2_F32 vec2_f32(F32 x);

Vec2_F32 operator+(Vec2_F32 v1, Vec2_F32 v2);
Vec2_F32 operator-(Vec2_F32 v1, Vec2_F32 v2);
Vec2_F32 operator*(Vec2_F32 v1, Vec2_F32 v2);
Vec2_F32 operator/(Vec2_F32 v1, Vec2_F32 v2);

Vec2_F32& operator+=(Vec2_F32& v1, Vec2_F32 v2);
Vec2_F32& operator-=(Vec2_F32& v1, Vec2_F32 v2);
Vec2_F32& operator*=(Vec2_F32& v1, Vec2_F32 v2);
Vec2_F32& operator/=(Vec2_F32& v1, Vec2_F32 v2);

Vec2_F32 operator+(Vec2_F32 v, F32 mult);
Vec2_F32 operator+(F32 mult, Vec2_F32 v);
Vec2_F32 operator-(Vec2_F32 v, F32 mult);
Vec2_F32 operator-(F32 mult, Vec2_F32 v);
Vec2_F32 operator*(Vec2_F32 v, F32 mult);
Vec2_F32 operator*(F32 mult, Vec2_F32 v);
Vec2_F32 operator/(Vec2_F32 v, F32 mult);
Vec2_F32 operator/(F32 mult, Vec2_F32 v);

Vec2_F32& operator+=(Vec2_F32& v, F32 mult);
Vec2_F32& operator-=(Vec2_F32& v, F32 mult);
Vec2_F32& operator*=(Vec2_F32& v, F32 mult);
Vec2_F32& operator/=(Vec2_F32& v, F32 mult);

B32 operator==(Vec2_F32 v1, Vec2_F32 v2);
B32 operator!=(Vec2_F32 v1, Vec2_F32 v2);

F32 vec2_f32_dot(Vec2_F32 v1, Vec2_F32 v2);
F32 vec2_f32_len_sq(Vec2_F32 v);
F32 vec2_f32_len(Vec2_F32 v);
Vec2_F32 vec2_f32_unit(Vec2_F32 v);
B32 vec2_f32_is_unit(Vec2_F32 v);

///////////////////////////////////////////////////////////
// Damian: Vec2_S32 stuff
//
Vec2_S32 vec2_s32(S32 x, S32 y);
Vec2_S32 vec2_s32(S32 x);
Vec2_S32 operator+(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator-(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator*(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator/(Vec2_S32 v1, Vec2_S32 v2);

///////////////////////////////////////////////////////////
// Damian: Vec2_U32 stuff
//
Vec2_U32 vec2_u32(U32 x, U32 y);
Vec2_U32 vec2_u32(U32 x);

///////////////////////////////////////////////////////////
// Damian: Vec3_F32 stuff 
//
Vec3_F32 vec3_f32(F32 r, F32 g, F32 b, F32 a);

Vec3_F32& operator*=(Vec3_F32& v, F32 x);
Vec3_F32& operator/=(Vec3_F32& v, F32 x);

///////////////////////////////////////////////////////////
// Damian: Vec4_F32 stuff
//

///////////////////////////////////////////////////////////
// Damian: Rect_F32
//

Rect rect_make(F32 x, F32 y, F32 width, F32 height)
{
  Rect result = {};
  result.x = x;
  result.y = y;
  result.width = width;
  result.height = height;
  return result;
}

Rect rect_from_min_point(F32 x, F32 y, F32 width, F32 height)
{
  Rect result = {};
  result.x = x;
  result.y = y;
  result.width     = width;
  result.height    = height;
  return result;
}

Rect rect_from_min_point_vec(Vec2_F32 min_point, F32 width, F32 height)
{
  Rect result = rect_from_min_point(min_point.x, min_point.y, width, height);
  return result;
} 

Rect rect_from_center(F32 x, F32 y, F32 width, F32 height)
{
  Rect result = {};
  result.x = x - (0.5f * width);
  result.y = y - (0.5f * height);
  result.width     = width;
  result.height    = height;
  return result;
}

Rect rect_from_center_vec(Vec2_F32 center_point, F32 width, F32 height)
{
  Rect result = rect_from_center(center_point.x, center_point.y, width, height);
  return result;
}

B32 rect_does_intersect(Rect rect1, Rect rect2)
{
  // TODO: redo this
  Vec2_F32 rect1_min_point = vec2_f32(rect1.x, rect1.y);
  Vec2_F32 rect2_min_point = vec2_f32(rect2.x, rect2.y);
  Vec2_F32 rect1_max = rect1_min_point + vec2_f32(rect1.width - 1, rect1.height - 1);
  Vec2_F32 rect2_max = rect2_min_point + vec2_f32(rect2.width - 1, rect2.height - 1);
  if (   rect1_min_point.y > rect2_max.y 
      || rect1_max.y < rect2_min_point.y
      || rect1_min_point.x > rect2_max.x
      || rect1_max.x < rect2_min_point.x
  ) {
      return false;
  }
  return true;
}

///////////////////////////////////////////////////////////
// Damian: Matrix stuff (DEBUG FOR NOW)
//
// NOTE: These are row based
struct Mat4x4_F32 {  
  F32 x[4][4];
};

typedef Mat4x4_F32 Mat4;

Mat4x4_F32 mat4x4_f32_identity()
{
  Mat4x4_F32 mat = {};
  mat.x[0][0] = 1;
  mat.x[1][1] = 1;
  mat.x[2][2] = 1;
  mat.x[3][3] = 1;
  return mat;
}

Mat4x4_F32 mat4x4_f32_scale(F32 x_scale, F32 y_scale, F32 z_scale) 
{
  Mat4x4_F32 result = mat4x4_f32_identity();
  result.x[0][0] *= x_scale;
  result.x[1][1] *= y_scale;
  result.x[2][2] *= z_scale;
  return result;
}

Mat4x4_F32 mat4x4_f32_ortho(F32 left, F32 right, F32 bottom, F32 top, F32 near_, F32 far_)
{
  Mat4x4_F32 result = mat4x4_f32_identity();
  result.x[0][0] = (2 / (right - left));
  result.x[1][1] = (2 / (top - bottom));
  result.x[2][2] = (-2 / (far_ - near_));
  result.x[3][3] = 1;

  result.x[0][3] = (-(right + left) / (right - left));
  result.x[1][3] = (-(top + bottom) / (top - bottom));
  result.x[2][3] = (-(far_ + near_) / (far_ - near_));

  return result;
}

Mat4x4_F32 mat4x4_f32_translate(F32 x, F32 y, F32 z)
{
  Mat4x4_F32 result = mat4x4_f32_identity();
  result.x[0][3] = x;
  result.x[1][3] = y;
  result.x[2][3] = z;
  return result;
}

// Mat4x4_F32 mat4x4_f32_transpose(Mat4x4_F32 mat) 
// {



// }

///////////////////////////////////////////////////////////
// Damian: THESE ARE HERE FOR NOW, I JUST NEED THEM,
//         BUT YET DONT KNOW WHERE TO PUT THEM
//
// Damian: I wanted them to be not caps, but if i define RED as Red,
//         then i start getting some win32 errors, cause in some structs they use
//         Red as a memeber. So for now i just put C_ in front of them.
//         TODO: Deal with this.
#define C_TRANSPARENT vec4_f32(0.0f, 0.0f, 0.0f, 0.0f)
#define C_BLACK       vec4_f32(0.0f, 0.0f, 0.0f, 1.0f)
#define C_WHITE       vec4_f32(1.0f, 1.0f, 1.0f, 1.0f)
#define C_RED         vec4_f32(1.0f, 0.0f, 0.0f, 1.0f)
#define C_GREEN       vec4_f32(0.0f, 1.0f, 0.0f, 1.0f)
#define C_BLUE        vec4_f32(0.0f, 0.0f, 1.0f, 1.0f)


#endif

























