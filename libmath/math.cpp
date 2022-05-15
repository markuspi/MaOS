#include "math.hpp"

#include <types.h>

constexpr u32 SIGN_MASK = 0x80000000;
constexpr u32 SIGN_MASK_INV = 0x7FFFFFFF;

// Based on https://stackoverflow.com/a/14100975
float atan2(float y, float x) {
    static const float b = 0.596227f;

    // Extract the sign bits
    u32 ux_s = SIGN_MASK & floatAsInt(x);
    u32 uy_s = SIGN_MASK & floatAsInt(y);

    // Determine the quadrant offset
    auto q = (float)((~ux_s & uy_s) >> 29 | ux_s >> 30);

    // Calculate the arctangent in the first quadrant
    float bxy_a = fabs(b * x * y);
    float num = bxy_a + y * y;
    float atan_1q = num / (x * x + bxy_a + num);

    // Translate it to the proper quadrant
    u32 uatan_2q = (ux_s ^ uy_s) | floatAsInt(atan_1q);
    return (q + intAsFloat(uatan_2q)) * M_PI_2;
}

float fmod(float x, float y) {
    unsigned short fpsr;
    do {
        asm volatile("fprem; fnstsw %%ax" : "+t"(x), "=a"(fpsr) : "u"(y));
    } while (fpsr & 0x400);
    return x;
}
