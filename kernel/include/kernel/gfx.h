#pragma once

#include "kernel/multiboot.h"
#include "kernel/types.h"

bool gfx_init(multiboot_t* mb);
void gfx_apply(uint32_t* buffer);

/// Quickly convert a fully saturated color to RGB
/// @param h hue in interval [0; 6)
u32 hsv2rgbfast(float h);

/// Convert hue, saturation, value to RGB
/// @param h hue in interval [0; 360)
/// @param s saturation in interval [0; 1)
/// @param v value in interval [0; 1)
u32 hsv2rgb(float h, float s, float v);
