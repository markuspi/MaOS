#include <stdbool.h>
#include <stddef.h>

#include "kernel/memory.h"
#include "kernel/vm.h"

void* kmalloc(size_t sz) {
    size_t n_pages = PAGE_ALIGN(sz) >> PAGE_BITS;
    return vm_alloc_kpages(n_pages);
}

void kfree(void* ptr) {
    PANIC("kree not implemented");
}
