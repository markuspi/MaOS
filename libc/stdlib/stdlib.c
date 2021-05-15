#include "stdlib.h"

#include <stddef.h>

#include "stdio.h"
#include "string.h"

#ifdef __is_libk
#include "kernel/vm.h"
int liballoc_lock() {
    return 0;
}

int liballoc_unlock() {
    return 0;
}

void* liballoc_alloc(int npages) {
    return vm_alloc_kpages(npages);
}

int liballoc_free(void* ptr, int npages) {
    vm_free_kpages(npages, ptr);
    return 0;
}
#endif
