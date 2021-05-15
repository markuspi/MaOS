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

    printf("Hello, World!!\n");

    void* x = vm_alloc_kpages(1);
    void* y = vm_alloc_kpages(1);
    vm_debug();
    vm_free_kpages(1, x);
    vm_debug();
    vm_free_kpages(1, y);
    vm_debug();
    // void* z = vm_alloc_kpages(1);

    printf("Done!\n");
}
