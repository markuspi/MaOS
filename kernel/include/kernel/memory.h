#pragma once

#include <types.h>

#include "kernel/common.h"

CPP_GUARD_START

#define PAGE_BITS 12
#define PAGE_SIZE (1 << PAGE_BITS)
#define PAGE_MASK (PAGE_SIZE - 1)
#define PAGE_ALIGN(addr) ALIGN_BITS_UP(addr, PAGE_BITS)

typedef enum { paging_state_uninitialized, paging_state_steal, paging_state_active } paging_state_t;

void pmm_init(paddr_t upper_bound);
err_t pmm_alloc(size_t n_frames, paddr_t* addr);
void pmm_free(size_t n_frames, paddr_t addr);
void pmm_debug();
void* pmm_get_border();

void* kmalloc(size_t sz);
void kfree(void* ptr);

CPP_GUARD_END
