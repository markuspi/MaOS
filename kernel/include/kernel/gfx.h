#pragma once

#include "kernel/multiboot.h"
#include "kernel/types.h"

bool gfx_init(multiboot_t* mb);
void gfx_apply(uint32_t* buf);

u32 hsv2rgbfast(float h);
u32 hsv2rgb(float h, float s, float v);
