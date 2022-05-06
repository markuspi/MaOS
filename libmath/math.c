#include "math.h"

#include <stdint.h>

static const uint32_t SIGN_MASK = 0x80000000;
static const uint32_t SIGN_MASK_INV = 0x7FFFFFFF;

#define FLOAT_AS_INT(num) (*(uint32_t*)&(num))
#define INT_AS_FLOAT(num) (*(float*)&(num))

// Based on https://stackoverflow.com/a/14100975
float atan2(float y, float x) {
    static const float b = 0.596227f;

    // Extract the sign bits
    uint32_t ux_s = SIGN_MASK & FLOAT_AS_INT(x);
    uint32_t uy_s = SIGN_MASK & FLOAT_AS_INT(y);

    // Determine the quadrant offset
    float q = (float)((~ux_s & uy_s) >> 29 | ux_s >> 30);

    // Calculate the arctangent in the first quadrant
    float bxy_a = fabs(b * x * y);
    float num = bxy_a + y * y;
    float atan_1q = num / (x * x + bxy_a + num);

    // Translate it to the proper quadrant
    uint32_t uatan_2q = (ux_s ^ uy_s) | FLOAT_AS_INT(atan_1q);
    return (q + INT_AS_FLOAT(uatan_2q)) * M_PI_2;
}

float sin(float x) {
    return x;
}
