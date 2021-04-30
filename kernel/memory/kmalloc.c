#include <stddef.h>
#include <stdbool.h>

#include "kernel/memory.h"

void* kmalloc_extended(size_t sz, paddr_t *phys, bool page_aligned)
{
    size_t n_pages = PAGE_ALIGN(sz) / PAGE_SIZE;
    return paging_steal(n_pages, phys);
}