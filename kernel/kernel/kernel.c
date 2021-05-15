#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

    printf("Hello, World!!\n");

    char* x = malloc(10);
    x[0] = 'a';
    free(x);

    printf("Done!\n");
}
