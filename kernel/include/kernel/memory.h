#pragma once

#include <stddef.h>
#include <stdbool.h>

#include "kernel/common.h"

#define PAGE_BITS 12
#define PAGE_SIZE (1 << PAGE_BITS)
#define PAGE_MASK (PAGE_SIZE - 1)
#define PAGE_ALIGN(addr) ((addr) & PAGE_MASK ? (((addr) >> PAGE_BITS) + 1) << PAGE_BITS : (addr))

typedef enum {
    pmm_state_uninitialized,
    pmm_state_steal,
    pmm_state_buddy
} pmm_state_t;


void pmm_bootstrap();
void pmm_init();
err_t pmm_alloc(size_t n_frames, paddr_t* addr);

void* kmalloc_extended(size_t sz, paddr_t *phys, bool page_aligned);
#define kmalloc(sz) kmalloc_extended(sz, NULL, false)

void paging_bootstrap();
void paging_init();
void* paging_steal(size_t n_pages, paddr_t* phys);
