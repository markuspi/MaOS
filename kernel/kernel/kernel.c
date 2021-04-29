#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "kernel/tables.h"
#include "kernel/tty.h"
#include "kernel/common.h"

void kernel_main()
{
    tty_init();
    descriptor_tables_init();
    
    tty_writestr("Hello, World!aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
    tty_writestr("Nice\n");

    tty_setcolor(0xB4);
    printf("1234567890\n");
    printf("%05d\n", -1);

    PANIC("BAD: %s", "message");
}
