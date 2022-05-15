#pragma once

#include "common.h"

#define MB_FLAG_FRAMEBUFFER (1 << 12)

typedef struct multiboot {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    paddr_t cmdline;
    u8 _unused[68];
    paddr_t framebuffer_addr;
    u32 framebuffer_addr_high;
    u32 framebuffer_pitch;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u8 framebuffer_bpp;
    u8 framebuffer_type;
} PACKED multiboot_t;
