
#include "kernel/common.h"

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
