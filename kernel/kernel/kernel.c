#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel/common.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/multiboot.h"
#include "kernel/serial.h"
#include "kernel/tables.h"
#include "kernel/timer.h"
#include "kernel/tty.h"
#include "kernel/vm.h"

static uint32_t hsv2rgb(float h, float s, float v) {
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

    uint8_t r, g, b;
    r = (uint8_t)((r_ + m) * 255.0f);
    g = (uint8_t)((g_ + m) * 255.0f);
    b = (uint8_t)((b_ + m) * 255.0f);
    return (r << 16) | (g << 8) | b;
}

void kernel_main(multiboot_t* ptr) {
    tty_init();
    descriptor_tables_init();
    serial_init(COM1);
    pmm_init(ptr->mem_upper * KiB + 1 * MiB);
    vm_init();

    // keyboard_init();
    timer_init();

    printf("Hello, World!!\n");

    printf("Multiboot flags: %012b\n", ptr->flags);
    printf("Screen: %dx%d %d bpp pitch: %d\n", ptr->framebuffer_width, ptr->framebuffer_height,
           ptr->framebuffer_bpp, ptr->framebuffer_pitch);


    if (ptr->flags & MB_FLAG_FRAMEBUFFER) {
        size_t pixels = ptr->framebuffer_width * ptr->framebuffer_height;
        size_t fb_size = pixels * ptr->framebuffer_bpp / 8;
        size_t fb_pages = fb_size / PAGE_SIZE;

        for (size_t i = 0; i < fb_pages; i++) {
            vm_map_page(current_as, (paddr_t)ptr->framebuffer_addr + i * PAGE_SIZE,
                        (vaddr_t)ptr->framebuffer_addr + i * PAGE_SIZE);
        }

        size_t width = ptr->framebuffer_width;
        size_t height = ptr->framebuffer_height;
        size_t depth = ptr->framebuffer_bpp / 8;
        size_t buffer_size = width * height * depth;
        uint32_t* back_buffer = malloc(buffer_size);

        const int w2 = width >> 1;
        const int h2 = height >> 1;
        const float factor = 360.0f / M_2_PI;
        float time = 0;


        while (true) {
            uint32_t* buf = back_buffer;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    float hue = fmod(factor * atan2((float)(x - w2), (float)(y
                    - h2)) + time, 360.0f);

                    u32 col = hsv2rgb(hue, 1.0f, 1.0f);
                    *buf++ = col;
                }
            }
            memcpy(ptr->framebuffer_addr, back_buffer, buffer_size);
            printf("x");
            time += 10.0f;

            sleep(100);
        }
    } else {
        tty_setcolor(tty_makecolor(TTY_COLOR_BLACK, TTY_COLOR_LIGHT_RED));
        printf("Graphics not supported!\n");
        tty_setcolor(TTY_COLOR_WHITE);
    }
}
