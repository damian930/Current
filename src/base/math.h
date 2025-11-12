#ifndef BASE_MATH_H
#define BASE_MATH_H

#include "base/core.h"

///////////////////////////////////////////////////////////
// Damian: Some regular macros
//
#define Max(x, y) (x > y ? x : y)
#define Min(x, y) (x < y ? x : y)

#define ValueToMin(value_to_min, possible_min) \
  do { if (value_to_min > possible_min) { value_to_min = possible_min; } } while(false);
#define ValueToMax(value_to_max, possible_max) \
  do { if (value_to_max < possible_max) { value_to_max = possible_max; } } while(false);

#define ValueToMinWithAction(value, possible_min, callback) \
  do { if (value > possible_min) { value = possible_min; callback; } } while(false);

#define Abs(Type, x) ( ((x) > Type{} ? (x) : (-(x))) )
#define Square(x) ((x) * (x))

struct Vec2_F32 {
  union {
    struct {
      F32 x;
      F32 y;
    };
    struct {
      F32 values[2]; // Maybe changing this to v2 would be nicer
    };
  };
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
  result.width  = width;
  result.height = height;
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

Rect rect_from_points(F32 x1, F32 y1, F32 x2, F32 y2)
{
  Rect rect = {};
  rect.x = x1;
  rect.y = y1;
  rect.width = x2 - x1;
  rect.height = y2 - y1;
  return rect;
}

Rect rect_from_points_vec(Vec2_F32 p1, Vec2_F32 p2)
{
  Rect rect = rect_from_points(p1.x, p1.y, p2.x, p2.y);
  return rect;
}

Vec2_F32 rect_pos(Rect rect)
{
  Vec2_F32 result = {};
  result.x = rect.x;
  result.y = rect.y;
  return result;
}

Vec2_F32 rect_dims(Rect rect)
{
  Vec2_F32 result = {};
  result.x = rect.width; 
  result.y = rect.height;
  return result;
}

B32 rect_does_intersect_with_point(Rect rect, Vec2_F32 p)
{
  B32 result = false;
  if (   rect.x <= p.x && p.x < rect.x + rect.width
      && rect.y <= p.y && p.y < rect.y + rect.height
  ) {
    result = true;
  }
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

// Primary colors
#define C_YELLOW      vec4_f32(1.0f, 1.0f, 0.0f, 1.0f)
#define C_CYAN        vec4_f32(0.0f, 1.0f, 1.0f, 1.0f)
#define C_MAGENTA     vec4_f32(1.0f, 0.0f, 1.0f, 1.0f)

// Common colors
#define C_ORANGE      vec4_f32(1.0f, 0.647f, 0.0f, 1.0f)
#define C_PURPLE      vec4_f32(0.5f, 0.0f, 0.5f, 1.0f)
#define C_PINK        vec4_f32(0.737f, 0.580f, 0.733f, 1.0f)
#define C_BROWN       vec4_f32(0.6f, 0.4f, 0.2f, 1.0f)
#define C_GRAY        vec4_f32(0.502f, 0.502f, 0.502f, 1.0f)
#define C_GREY        vec4_f32(0.502f, 0.502f, 0.502f, 1.0f)

// Shades
#define C_LIGHT_GRAY  vec4_f32(0.827f, 0.827f, 0.827f, 1.0f)
#define C_LIGHT_GREY  vec4_f32(0.827f, 0.827f, 0.827f, 1.0f)
#define C_DARK_GRAY   vec4_f32(0.663f, 0.663f, 0.663f, 1.0f)
#define C_DARK_GREY   vec4_f32(0.663f, 0.663f, 0.663f, 1.0f)

// Additional useful colors
#define C_LIME        vec4_f32(0.0f, 1.0f, 0.0f, 1.0f)
#define C_NAVY        vec4_f32(0.0f, 0.0f, 0.502f, 1.0f)
#define C_TEAL        vec4_f32(0.0f, 0.502f, 0.502f, 1.0f)
#define C_OLIVE       vec4_f32(0.502f, 0.502f, 0.0f, 1.0f)
#define C_MAROON      vec4_f32(0.502f, 0.0f, 0.0f, 1.0f)
#define C_SILVER      vec4_f32(0.753f, 0.753f, 0.753f, 1.0f)
#define C_GOLD        vec4_f32(1.0f, 0.843f, 0.0f, 1.0f)

// Damian: Max here is ussually used as the non equals bound
struct Range_F32 {
  F32 min;
  F32 max;
};

struct Range_U32 {
  U32 min;
  U32 max;
};
#define ForEachRangeU32(it_name, range) for (U32 it_name = range.min; it_name < range.max; it_name += 1)

Range_F32 range_f32(F32 min, F32 max)
{
  Range_F32 range = {};
  range.min = min;
  range.max = max;
  return range;
}

Range_U32 range_u32(U32 min, U32 max)
{
  Range_U32 range = {};
  range.min = min;
  range.max = max;
  return range;
}

U32 range_u32_count(Range_U32 range)
{
  U32 count = range.max - range.min;
  return count; 
}

U32 range_u32_within(Range_U32 range, U32 value)
{
  B32 result = (range.min <= value && value < range.max);
  return result;
}

// TODO: This has to be moved
Vec4_F32 vec4_f32(F32 r, F32 g, F32 b, F32 a);
Color color_make(F32 r, F32 g, F32 b, F32 a)
{
  Color color = vec4_f32(r, g, b, a);
  return color;
}

Color color_set_a(Color color, F32 new_a)
{
  Color result = color;
  result.a = new_a;
  return result;
}

#endif

























