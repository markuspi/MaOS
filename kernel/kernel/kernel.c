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

void kernel_main(multiboot_t* ptr) {
    tty_init();
    descriptor_tables_init();
    serial_init(COM1);
    pmm_init(ptr->mem_upper * KiB + 1 * MiB);
    vm_init();

    //keyboard_init();
    timer_init();

    printf("Hello, World!!\n");

    printf("Multiboot flags: %012b\n", ptr->flags);

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
        size_t i = 0;
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                u8 r = y * 255 / height;
                u8 g = x * 255 / width;
                u32 col = (r << 16) | (g << 8) | 0;
                ptr->framebuffer_addr[i++] = col;
            }
        }
    } else {
        tty_setcolor(tty_makecolor(TTY_COLOR_BLACK, TTY_COLOR_LIGHT_RED));
        printf("Graphics not supported!\n");
        tty_setcolor(TTY_COLOR_WHITE);
    }
}
