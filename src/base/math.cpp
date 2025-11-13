#ifndef BASE_MATH_CPP
#define BASE_MATH_CPP

#include <math.h>
#include "math.h"

///////////////////////////////////////////////////////////
// Damian: Vec2_F32 stuff
//
Vec2_F32 vec2_f32(F32 x, F32 y)
{
    Vec2_F32 result = {};
    result.x = x;
    result.y = y;
    return result;
}

Vec2_F32 vec2_f32(F32 x)
{
    Vec2_F32 result = {};
    result.x = x;
    result.y = x;
    return result;
}

Vec2_F32 operator+(Vec2_F32 v1, Vec2_F32 v2)
{
    Vec2_F32 result = {};
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

Vec2_F32 operator-(Vec2_F32 v1, Vec2_F32 v2)
{
    Vec2_F32 result = {};
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

Vec2_F32 operator*(Vec2_F32 v1, Vec2_F32 v2)
{
    Vec2_F32 result = {};
    result.x = v1.x * v2.x;
    result.y = v1.y * v2.y;
    return result;
}

Vec2_F32 operator/(Vec2_F32 v1, Vec2_F32 v2)
{
    Vec2_F32 result = {};
    result.x = v1.x / v2.x;
    result.y = v1.y / v2.y;
    return result;
}

Vec2_F32& operator+=(Vec2_F32& v1, Vec2_F32 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

Vec2_F32& operator-=(Vec2_F32& v1, Vec2_F32 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}

Vec2_F32& operator*=(Vec2_F32& v1, Vec2_F32 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    return v1;
}

Vec2_F32& operator/=(Vec2_F32& v1, Vec2_F32 v2)
{
    v1.x /= v2.x;
    v1.y /= v2.y;
    return v1;
}

Vec2_F32 operator+(Vec2_F32 v, F32 mult)
{
    Vec2_F32 result = v;
    result.x += mult;
    result.y += mult;
    return result;
}

Vec2_F32 operator+(F32 mult, Vec2_F32 v)
{
    Vec2_F32 result = v + mult;
    return result;
}

Vec2_F32 operator-(Vec2_F32 v, F32 mult)
{
    Vec2_F32 result = v;
    result.x -= mult;
    result.y -= mult;
    return result;
}

Vec2_F32 operator-(F32 mult, Vec2_F32 v)
{
    Vec2_F32 result = v - mult;
    return result;
}

Vec2_F32 operator*(Vec2_F32 v, F32 mult)
{
    Vec2_F32 result = v;
    result.x *= mult;
    result.y *= mult;
    return result;
}

Vec2_F32 operator*(F32 mult, Vec2_F32 v)
{
    Vec2_F32 result = v * mult;
    return result;
}

Vec2_F32 operator/(Vec2_F32 v, F32 mult)
{
    Vec2_F32 result = v;
    result.x /= mult;
    result.y /= mult;
    return result;
}

Vec2_F32 operator/(F32 mult, Vec2_F32 v)
{
    Vec2_F32 result = v / mult;
    return result;
}


Vec2_F32& operator+=(Vec2_F32& v, F32 mult)
{
    v = v + mult;
    return v;
}

Vec2_F32& operator-=(Vec2_F32& v, F32 mult)
{
    v = v - mult;
    return v;
}

Vec2_F32& operator*=(Vec2_F32& v, F32 mult)
{
    v = v * mult;
    return v;
}

Vec2_F32& operator/=(Vec2_F32& v, F32 mult)
{
    v = v / mult;
    return v;
}

B32 operator==(Vec2_F32 v1, Vec2_F32 v2)
{
    B32 result = (v1.x == v2.x) && 
                 (v1.y == v2.y);
    return result;
}

B32 operator!=(Vec2_F32 v1, Vec2_F32 v2)
{
    B32 result = !(v1 == v2);
    return result;
}

F32 vec2_f32_dot(Vec2_F32 v1, Vec2_F32 v2)
{
    F32 result = (v1.x * v2.x) + (v1.y * v2.y);
    return result;
}

F32 vec2_f32_len_sq(Vec2_F32 v)
{
    F32 result = (v.x * v.x) + (v.y * v.y);
    return result;
}

// TODO: use the instrinsic square root here insted of the c runtime lin sqrt
F32 vec2_f32_len(Vec2_F32 v)
{
    F32 result = sqrtf(vec2_f32_len_sq(v));
    return result;
}

Vec2_F32 vec2_f32_unit(Vec2_F32 v)
{
    Assert(v != vec2_f32(0.0f, 0.0f));
    Vec2_F32 result = v / vec2_f32_len(v);
    return result;
}

B32 vec2_f32_is_unit(Vec2_F32 v)
{
    // Dont know if this is possible with floats and the inprecision
    Vec2_F32 tester = vec2_f32_unit(v);
    return v == tester;
}

///////////////////////////////////////////////////////////
// Damian: Vec2_S32 stuff 
//
Vec2_S32 vec2_s32(S32 x, S32 y)
{
    Vec2_S32 result = {};
    result.x = x;
    result.y = y;
    return result;
}

Vec2_S32 vec2_s32(S32 x)
{
    Vec2_S32 result = {};
    result.x = x;
    result.y = x;
    return result;
}

Vec2_S32 operator+(Vec2_S32 v1, Vec2_S32 v2)
{
    Vec2_S32 result = {};
    result.x = v1.x + v2.x;
    result.y = v1.y + v2.y;
    return result;
}

Vec2_S32 operator-(Vec2_S32 v1, Vec2_S32 v2)
{
    Vec2_S32 result = {};
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    return result;
}

Vec2_S32 operator*(Vec2_S32 v1, Vec2_S32 v2)
{
    Vec2_S32 result = {};
    result.x = v1.x * v2.x;
    result.y = v1.y * v2.y;
    return result;
}

Vec2_S32 operator/(Vec2_S32 v1, Vec2_S32 v2)
{
    Vec2_S32 result = {};
    result.x = v1.x / v2.x;
    result.y = v1.y / v2.y;
    return result;
}

///////////////////////////////////////////////////////////
// Damian: Vec2_U32 stuff
//
Vec2_U32 vec2_u32(U32 x, U32 y)
{
    Vec2_U32 result = {};
    result.x = x;
    result.y = y;
    return result;
}
Vec2_U32 vec2_u32(U32 x)
{
    Vec2_U32 result = {};
    result.x = x;
    result.y = x;
    return result;
}

// Vec2_U32 operator+(Vec2_U32 v1, Vec2_U32 v2);
// Vec2_U32 operator-(Vec2_U32 v1, Vec2_U32 v2);
// Vec2_U32 operator*(Vec2_U32 v1, Vec2_U32 v2);
// Vec2_U32 operator/(Vec2_U32 v1, Vec2_U32 v2);

///////////////////////////////////////////////////////////
// Damian: Vec3_F32 stuff
//
Vec3_F32 vec3_f32(F32 r, F32 g, F32 b)
{
    Vec3_F32 result = {};
    result.r = r;
    result.g = g;
    result.b = b;
    return result;
}

Vec3_F32& operator*=(Vec3_F32& v, F32 x)
{
    v.x *= x;
    v.y *= x;
    v.z *= x;
    return v;
}

Vec3_F32& operator/=(Vec3_F32& v, F32 x)
{
    v.x /= x;
    v.y /= x;
    v.z /= x;
    return v;
}

///////////////////////////////////////////////////////////
// Damian: Vec4_F32 stuff
//
Vec4_F32 vec4_f32(F32 r, F32 g, F32 b, F32 a)
{
    Vec4_F32 result = {};
    result.r = r;
    result.g = g;
    result.b = b;
    result.a = a;
    return result;
}

///////////////////////////////////////////////////////////
// Damian: Rect stuff
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
// Damian: Mat4x4 stuff
//
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

///////////////////////////////////////////////////////////
// Damian: Range_F32 stuff
//
Range_F32 range_f32(F32 min, F32 max)
{
  Range_F32 range = {};
  range.min = min;
  range.max = max;
  return range;
}

///////////////////////////////////////////////////////////
// Damian: Range_U32 stuff
//
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

///////////////////////////////////////////////////////////
// Damian: Bounding box stuff 
//
Bounding_box bounding_box_make(F32 x0, F32 y0, F32 x1, F32 y1)
{
  Bounding_box box = {};
  box.x0 = x0;
  box.y0 = y0;
  box.x1 = x1;
  box.y1 = y1;
  return box;
}

///////////////////////////////////////////////////////////
// Damian: Other mathy functions
//
U64 align_up(U64 value, U64 alignment)
{
  U64 rounded = (value - 1 + alignment) - ((value - 1 + alignment) % alignment); 
  return rounded;
}

U8* align_up_p(U8* p, U64 alignment)
{
  U64 p_address = (U64)p;
  U64 p_address_aligned = align_up(p_address, alignment);
  U8* aligned_p = (U8*)p_address_aligned;
  return aligned_p; 
}

///////////////////////////////////////////////////////////
// Damian: Colors stuff
//
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