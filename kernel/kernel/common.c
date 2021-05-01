
#include "kernel/common.h"

#include <stdarg.h>
#include <stdio.h>

#include "kernel/tty.h"

static void print_line() {
    puts(
        "\n--------------------------------------------------------------------"
        "-----------\n");
}

void kernel_panic(const char* filename, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);

    tty_setcolor(12);
    print_line();
    printf("KERNEL PANIC at %s:%d\n", filename, (int32_t)line);
    vprintf(format, args);
    print_line();

    while (1) {
        halt();
    }

    va_end(args);
}

void nopN(size_t n) {
    for (size_t i = 0; i < n; i++) {
        nop();
    }
}
