#include <stdbool.h>
#include <stddef.h>

#include "kernel/memory.h"

void* kmalloc_extended(size_t sz, paddr_t* phys, bool page_aligned) {
    size_t n_pages = PAGE_ALIGN(sz) / PAGE_SIZE;
    return paging_alloc(n_pages, phys);
}
