
#include "kernel/common.h"

void inline halt() {
    asm volatile ("hlt");
}
