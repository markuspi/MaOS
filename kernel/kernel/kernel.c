#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "kernel/memory.h"
#include "kernel/tables.h"
#include "kernel/tty.h"
#include "kernel/common.h"

void kernel_main()
{
    tty_init();
    descriptor_tables_init();
    pmm_bootstrap();
    paging_bootstrap();

    pmm_init();
    
    printf("Hello, World!\n");

}
