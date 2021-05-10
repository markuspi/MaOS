#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "kernel/common.h"
#include "kernel/hello.h"
#include "kernel/keyboard.h"
#include "kernel/memory.h"
#include "kernel/tables.h"
#include "kernel/tty.h"
#include "kernel/vm.h"

void kernel_main() {
    tty_init();
    descriptor_tables_init();
    pmm_bootstrap();
    vm_init();
    pmm_init();

    keyboard_init();

    printf("Hello, World!\n");

    kmalloc(12);
    kmalloc(2090);

    printf("Done!\n");
}
