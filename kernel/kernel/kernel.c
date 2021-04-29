#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "kernel/tables.h"
#include "kernel/tty.h"

void kernel_main()
{
    tty_init();
    descriptor_tables_init();
    
    tty_writestr("Hello, World!aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
    tty_writestr("Nice\n");
}
