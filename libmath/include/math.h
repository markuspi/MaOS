#pragma once

#define M_PI   3.141592653589793f
#define M_2_PI 6.283185307179586f
#define M_PI_2 1.570796326794897f

/// arctan2 in range [0; 2pi)
float atan2(float y, float x);

// assembly functions

/// floating-point modulo
/// x % y
float fmod(float x, float y);

float fsin(float x);
float fcos(float x);
float fsqrt(float x);

/// Calculate the absolute value of a float
float fabs(float num);