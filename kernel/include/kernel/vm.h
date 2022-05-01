#pragma once

#include <string.h>

#include "kernel/common.h"
#include "kernel/memory.h"

typedef struct vm_free_bucket_ {
    size_t pageno;
    size_t size;
    struct vm_free_bucket_* next;
    struct vm_free_bucket_* prev;
} vm_free_bucket_t;

typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;  // page-level write-through
    uint32_t pcd : 1;  // page-level cache disable
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t pat : 1;
    uint32_t global : 1;
    uint32_t ignored : 3;
    uint32_t frame : 20;
} pte_t;

/// a page table contains 1024 dword entries
/// so its size is 4 KiB
/// each entry handles one page (4 KiB)
/// so each table handles 4 MiB
typedef struct {
    pte_t entries[1024];
} page_table_t;

typedef struct {
    uint32_t present : 1;
    uint32_t rw : 1;
    uint32_t user : 1;
    uint32_t pwt : 1;  // page-level write-through
    uint32_t pcd : 1;  // page-level cache disable
    uint32_t accessed : 1;
    uint32_t dirty : 1;
    uint32_t page_size : 1;
    uint32_t ignored : 4;
    uint32_t page_table : 20;  // assumes 4KiB aligned
} pde_t;

/// page directory contains 1024 pointers to tables
/// so its size is 4 KiB
typedef struct {
    pde_t entries[1024];
} page_directory_t;

typedef struct {
    page_directory_t* directory;
    paddr_t directory_phys;
    page_table_t* tables[1024];
} as_t;

void vm_init();
void* vm_alloc_kpages(size_t pages);
void vm_free_kpages(size_t pages, void* addr);
void vm_map_page(as_t* as, paddr_t paddr, vaddr_t vaddr);
void vm_debug();

extern as_t* current_as;

// defined in paging.asm
void set_page_directory(paddr_t addr);
