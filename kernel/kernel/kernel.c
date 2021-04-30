#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "kernel/memory.h"
#include "kernel/tables.h"
#include "kernel/tty.h"
#include "kernel/common.h"
#include "kernel/keyboard.h"

void kernel_main()
{
    tty_init();
    descriptor_tables_init();
    pmm_bootstrap();
    paging_bootstrap();
    pmm_init();

    keyboard_init();
    
    printf("Hello, World!\n");

    kmalloc(12);
    kmalloc(2090);

    while (1)
    {
        nopN(100);
    }
}
