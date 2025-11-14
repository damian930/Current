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

struct Mat4x4_F32 {  
  F32 x[4][4];
};
typedef Mat4x4_F32 Mat4;

struct Rect {
  F32 x;
  F32 y;
  F32 width;
  F32 height; 
};

// Damian: Max here is ussually used as the non inclusive bound
struct Range_F32 {
  F32 min;
  F32 max;
};

// Damian: Max here is ussually used as the non inclusive bound
struct Range_U32 {
  U32 min;
  U32 max;
};
#define ForEachRangeU32(it_name, range) for (U32 it_name = range.min; it_name < range.max; it_name += 1)

// Damian: Bounded box
struct Bounding_box {
  F32 x0;
  F32 y0;
  F32 x1; // Non inclusive
  F32 y1; // Non inclusive
};

// Damian: Vec2_F32 stuff
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

// Damian: Vec2_S32 stuff
Vec2_S32 vec2_s32(S32 x, S32 y);
Vec2_S32 vec2_s32(S32 x);
Vec2_S32 operator+(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator-(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator*(Vec2_S32 v1, Vec2_S32 v2);
Vec2_S32 operator/(Vec2_S32 v1, Vec2_S32 v2);

// Damian: Vec2_U32 stuff
Vec2_U32 vec2_u32(U32 x, U32 y);
Vec2_U32 vec2_u32(U32 x);

// Damian: Vec3_F32 stuff 
Vec3_F32 vec3_f32(F32 r, F32 g, F32 b, F32 a);

Vec3_F32& operator*=(Vec3_F32& v, F32 x);
Vec3_F32& operator/=(Vec3_F32& v, F32 x);

// Damian: Vec4_F32 stuff
Vec4_F32 vec4_f32(F32 r, F32 g, F32 b, F32 a);

// Damian: Rect stuff
Rect rect_make(F32 x, F32 y, F32 width, F32 height);
Rect rect_from_min_point(F32 x, F32 y, F32 width, F32 height);
Rect rect_from_min_point_vec(Vec2_F32 min_point, F32 width, F32 height);
Rect rect_from_center(F32 x, F32 y, F32 width, F32 height);
Rect rect_from_center_vec(Vec2_F32 center_point, F32 width, F32 height);
Rect rect_from_points(F32 x1, F32 y1, F32 x2, F32 y2);
Rect rect_from_points_vec(Vec2_F32 p1, Vec2_F32 p2);
Rect rect_from_bbox(Bounding_box b_box);
Vec2_F32 rect_pos(Rect rect);
Vec2_F32 rect_dims(Rect rect);
B32 rect_does_intersect_with_point(Rect rect, Vec2_F32 p);
B32 rect_does_intersect(Rect rect1, Rect rect2);

// Damian: Mat4x4 stuff
Mat4x4_F32 mat4x4_f32_identity();
Mat4x4_F32 mat4x4_f32_scale(F32 x_scale, F32 y_scale, F32 z_scale);
Mat4x4_F32 mat4x4_f32_ortho(F32 left, F32 right, F32 bottom, F32 top, F32 near_, F32 far_);
Mat4x4_F32 mat4x4_f32_translate(F32 x, F32 y, F32 z);

// Damian: Range_F32 stuff
Range_F32 range_f32(F32 min, F32 max);

// Damian: Range_U32 stuff
Range_U32 range_u32(U32 min, U32 max);
U32 range_u32_count(Range_U32 range);
U32 range_u32_within(Range_U32 range, U32 value);

// Damian: Bounding box stuff
Bounding_box bounding_box_make(F32 x0, F32 y0, F32 x1, F32 y1);

// Damian: Other mathy functions
U64 align_up(U64 value, U64 alignment);
U8* align_up_p(U8* p, U64 alignment);

// Damian: Colors
typedef Vec4_F32 Color;

// TODO: Remove C_ from there names, i added it to not have name conflics with drawing api for windows
#define C_TRANSPARENT vec4_f32(0.0f, 0.0f, 0.0f, 0.0f)
#define C_BLACK       vec4_f32(0.0f, 0.0f, 0.0f, 1.0f)
#define C_WHITE       vec4_f32(1.0f, 1.0f, 1.0f, 1.0f)
#define C_RED         vec4_f32(1.0f, 0.0f, 0.0f, 1.0f)
#define C_GREEN       vec4_f32(0.0f, 1.0f, 0.0f, 1.0f)
#define C_BLUE        vec4_f32(0.0f, 0.0f, 1.0f, 1.0f)

#define C_YELLOW      vec4_f32(1.0f, 1.0f, 0.0f, 1.0f)
#define C_MAGENTA     vec4_f32(1.0f, 0.0f, 1.0f, 1.0f)

#define C_PURPLE      vec4_f32(0.5f, 0.0f, 0.5f, 1.0f)
#define C_BROWN       vec4_f32(0.6f, 0.4f, 0.2f, 1.0f)
#define C_GRAY        vec4_f32(0.502f, 0.502f, 0.502f, 1.0f)
#define C_GREY        C_GRAY

#define C_LIGHT_GREEN vec4_f32(0.47f, 0.67f, 0.50f, 1.0f)
// #define C_BETTER_RED  vec4_f32(0.75f, 0.22f, 0.25f, 1.0f)

Color color_make(F32 r, F32 g, F32 b, F32 a);
Color color_set_a(Color color, F32 new_a);

#endif

























