
#include <types.h>
#include <math.hpp>

#include "kernel/gfx.hpp"

/// h: [0; 6)
u32 hsv2rgbfast(float h) {
    float x = 1.0f - fabs(fmod(h, 2.0f) - 1.0f);
    u8 r = 0, g = 0, b = 0;
    switch (((u32) h) % 6) {
        case 0:
            r = 255;
            g = (u8) (x * 255.0f);
            break;
        case 1:
            r = (u8) (x * 255.0f);
            g = 255;
            break;
        case 2:
            g = 255;
            b = (u8) (x * 255.0f);
            break;
        case 3:
            g = (u8) (x * 255.0f);
            b = 255;
            break;
        case 4:
            r = (u8) (x * 255.0f);
            b = 255;
            break;
        default:
            r = 255;
            b = (u8) (x * 255.0f);
            break;
    }
    return (r << 16) | (g << 8) | b;
}

u32 hsv2rgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    float r_ = 0, g_ = 0, b_ = 0;
    if (h < 60) {
        r_ = c;
        g_ = x;
    } else if (h < 120) {
        r_ = x;
        g_ = c;
    } else if (h < 180) {
        g_ = c;
        b_ = x;
    } else if (h < 240) {
        g_ = x;
        b_ = c;
    } else if (h < 300) {
        r_ = x;
        b_ = c;
    } else {
        r_ = c;
        b_ = x;
    }

    u8 r, g, b;
    r = (u8)((r_ + m) * 255.0f);
    g = (u8)((g_ + m) * 255.0f);
    b = (u8)((b_ + m) * 255.0f);
    return (r << 16) | (g << 8) | b;
}
