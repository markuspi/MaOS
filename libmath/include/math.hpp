#pragma once

#include <types.h>
#include <utils.h>

constexpr float M_PI = 3.141592653589793f;
constexpr float M_2_PI = 6.283185307179586f;
constexpr float M_PI_2 = 1.570796326794897f;

/// arctan2 in range [0; 2pi)
float atan2(float y, float x);

/// floating-point modulo
/// @return x % y
float fmod(float x, float y);

CPP_GUARD_START

float fsin(float x);
float fcos(float x);
float fsqrt(float x);

/// floating-point absolute value
/// @return |num|
float fabs(float num);

CPP_GUARD_END

/// bit-cast a floating-point number to an integer
static inline u32 floatAsInt(float num) {
    return *reinterpret_cast<u32*>(&num);
}

/// bit-cast an integer to a floating-point number
static inline float intAsFloat(u32 num) {
    return *reinterpret_cast<float*>(&num);
}
