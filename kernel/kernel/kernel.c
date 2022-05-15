#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/common.h"
#include "kernel/gfx.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/multiboot.h"
#include "kernel/pci.h"
#include "kernel/serial.h"
#include "kernel/tables.h"
#include "kernel/timer.h"
#include "kernel/tty.h"
#include "kernel/vm.h"

void kernel_main(multiboot_t* ptr) {
    tty_init();
    descriptor_tables_init();
    serial_init(COM1);
    pmm_init(ptr->mem_upper * KiB + 1 * MiB);
    vm_init();

    // keyboard_init();
    timer_init();

    pci_init();

    printf("Hello, World!!\n");

    printf("Multiboot flags: %012b\n", ptr->flags);
    printf("Screen: %dx%d %d bpp pitch: %d\n", ptr->framebuffer_width, ptr->framebuffer_height,
           ptr->framebuffer_bpp, ptr->framebuffer_pitch);


    if (gfx_init(ptr)) {
        size_t width = ptr->framebuffer_width;
        size_t height = ptr->framebuffer_height;
        size_t depth = ptr->framebuffer_bpp / 8;
        size_t buffer_size = width * height * depth;
        uint32_t* const back_buffer = malloc(buffer_size);

        const float factor = 6.0f / M_2_PI;
        float time = 10;

        while (true) {
            float scale = fsin(time) / 50.0f;
            uint32_t* buf = back_buffer;
            for (int y = 0; y < height; y++) {
                float hue = fmod(factor * fsin(scale * (float) y) + time, 6.0f);
                for (int x = 0; x < width; x++) {
                    u32 col = hsv2rgbfast(hue + (float) x * 0.01f);
                    *buf++ = col;
                }
            }
            gfx_apply(back_buffer);
            time += 0.1f;

            sleep(100);
        }
    } else {
        tty_setcolor(tty_makecolor(TTY_COLOR_BLACK, TTY_COLOR_LIGHT_RED));
        printf("Graphics not supported!\n");
        tty_setcolor(TTY_COLOR_WHITE);
    }

    printf("Done\n");
}
