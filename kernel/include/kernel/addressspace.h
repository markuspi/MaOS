#pragma once

#include <string.h>

#include "kernel/common.h"
#include "kernel/memory.h"

extern void set_page_directory(paddr_t dir_phys);

typedef struct as_free_list {
    size_t pagenr;
    size_t size;
    struct as_free_list* next;
    struct as_free_list* prev;
} as_free_list_t;

err_t as_reserve_pages(as_free_list_t* list, size_t pages, size_t* pageno_out) {
    while (list) {
        if (list->size >= pages) {
            *pageno_out = list->pagenr;
            list->pagenr += pages;
            list->size -= pages;
            return E_OK;
        }
        list = list->next;
    }
    return E_NOMEM;
}

void as_return_pages(as_free_list_t* list, size_t pages, size_t pageno) {
    while (list && list->pagenr <= pageno) {
        list = list->next;
    }
    size_t before = pageno - list->prev->pagenr;
    size_t after = list->pagenr - (pageno + pages);
    if (after > 0 && before > 0) {
    } else if (after)
}

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
    uint32_t ignored : 6;
    uint32_t page_table : 20;  // assumes 4KiB aligned
} pde_t;

typedef struct {
    pde_t entries[1024];
} page_directory_t;

typedef struct {
    as_free_list_t* userspace_list;
    page_directory_t* directory;
    paddr_t directory_phys;
    page_table_t* tables[1024];
} as_t;

void as_bootstrap() {}

as_t* as_create() {
    as_t* as = kmalloc(sizeof(as_t));
    if (as == NULL) {
        return NULL;
    }
    memset(as, 0, sizeof(as_t));

    paddr_t dir_phys;
    page_directory_t* dir = kmalloc_extended(sizeof(page_directory_t), &dir_phys, true);
    if (dir == NULL) {
        return NULL;
    }

    as->directory = dir;
    as->directory_phys = dir_phys;

    return as;
}

void as_activate(as_t* as) {
    set_page_directory(as->directory_phys);
}

void* alloc_kpages(size_t npages) {}