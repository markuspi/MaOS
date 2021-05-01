#if defined(__is_libk)
#include <kernel/tty.h>
#endif

#include "stdio.h"

int putchar(int c) {
#if defined(__is_libk)
    tty_putchar((char)c);
#else
    // TODO implement putchar for userspace
#endif
}
