
#include "kernel/common.h"

#include <stdarg.h>
#include <stdio.h>

#include "kernel/tty.h"

uint32_t dbflags = 0;

void inline halt() {
    asm volatile("hlt");
}

void inline nop() {
    asm volatile("nop");
}

void inline sti() {
    asm volatile("sti");
}

void inline cli() {
    asm volatile("cli");
}

static void print_line() {
    puts(
        "\n--------------------------------------------------------------------"
        "-----------\n");
}

static void print_stacktrace() {
    volatile uint32_t x = 0xDEADBEEF;

    for (int i = 0; i < 20; i++) {
        volatile uint32_t* x = ((uint32_t *) &x) + i;
        printf("%08x: %08x\n", x, *x);
    }

}

void kernel_panic(const char* filename, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);

    tty_setcolor(12);
    print_line();
    printf("KERNEL PANIC at %s:%d\n", filename, (int32_t)line);
    vprintf(format, args);
    print_line();
    //print_stacktrace();
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
