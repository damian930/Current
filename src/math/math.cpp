#ifndef MATH_CPP
#define MATH_CPP

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




#endif