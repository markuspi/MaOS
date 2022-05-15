#include "kernel/gfx.hpp"

#include <stdio.h>
#include <string.h>
#include <types.h>

#include "kernel/multiboot.h"
#include "kernel/vm.h"

static bool gfx_enabled = false;
static uint32_t* gfx_framebuffer = (uint32_t*) 0xFF000000;
static size_t gfx_fb_size;

bool gfx_init(multiboot_t *mb) {
    if (!(mb->flags & MB_FLAG_FRAMEBUFFER)) {
        return false;
    }

    if (mb->framebuffer_type == 2) {
        DEBUG(DB_GFX, "[GFX] Framebuffer type is 2: text mode\n");
        return false;
    }

    size_t pixels = mb->framebuffer_width * mb->framebuffer_height;
    gfx_fb_size = pixels * mb->framebuffer_bpp / 8;
    vm_dma(mb->framebuffer_addr, (vaddr_t)gfx_framebuffer, gfx_fb_size);

    memset(gfx_framebuffer, 100, gfx_fb_size);

    gfx_enabled = true;
    return true;
}

void gfx_apply(uint32_t *buffer) {
    memcpy(gfx_framebuffer, buffer, gfx_fb_size);
}
