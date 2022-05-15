
#include "kernel/vm.h"

#include <string.h>

#include "kernel/common.h"
#include "kernel/memory.h"
#include "kernel/linkervars.h"

static vm_free_bucket_t* vm_kernel_buckets;
static vm_free_bucket_t* vm_waiting_list;
static void* vm_steal_next = NULL;
static size_t vm_steal_remaining = 0;

static vm_free_bucket_t vm_first_bucket;
static as_t vm_first_as;

extern page_directory_t KERNEL_PAGE_DIRECTORY;
extern page_table_t KERNEL_PAGE_TABLES[1024];

// defined in boot.asm

as_t* current_as = NULL;

void* vm_kernel_stealmem(size_t size);

void ll_remove_item(vm_free_bucket_t** list, vm_free_bucket_t* item) {
    if (item->next) {
        item->next->prev = item->prev;
    }

    if (item->prev) {
        item->prev->next = item->next;
    } else {
        // item was the first in the list
        *list = item->next;
    }

    item->next = NULL;
    item->prev = NULL;
}

void ll_insert_before(vm_free_bucket_t** list, vm_free_bucket_t* newitem,
                      vm_free_bucket_t* olditem) {
    newitem->next = olditem;
    newitem->prev = olditem->prev;

    if (olditem->prev) {
        olditem->prev->next = newitem;
    } else {
        // olditem was first in list
        *list = newitem;
    }

    olditem->prev = newitem;
}

void ll_append_notempty(vm_free_bucket_t* newitem, vm_free_bucket_t* olditem) {
    ASSERT(olditem);
    olditem->next = newitem;
    newitem->prev = olditem;
}

/** pushes an item to the beginning of a list */
void ll_push_item(vm_free_bucket_t** list, vm_free_bucket_t* item) {
    ASSERT(item->prev == NULL);
    ASSERT(item->next == NULL);

    vm_free_bucket_t* old_first = *list;
    if (old_first) {
        old_first->prev = item;
        item->next = old_first;
    }

    *list = item;
}

/** pops an item from the beginning of a list */
vm_free_bucket_t* ll_pop_item(vm_free_bucket_t** list) {
    vm_free_bucket_t* item = *list;

    if (item) {
        ASSERT(item->prev == NULL);
        *list = item->next;
        item->next = NULL;
    }

    return item;
}

paddr_t virtual2phys(void* ptr) {
    return (paddr_t) ((vaddr_t) ptr - (vaddr_t) &KERNEL_OFFSET);
}

static void as_map_kernel_table(as_t* as, size_t index, pde_t flags) {
    page_table_t* table = &KERNEL_PAGE_TABLES[index];
    ASSERT((vaddr_t) table == PAGE_ALIGN((vaddr_t) table), "Table must be page-aligned");
    flags.page_table = virtual2phys(table) >> PAGE_BITS;
    as->directory->entries[index] = flags;
    as->tables[index] = table;
}

static void switch_as(as_t* new_as) {
    current_as = new_as;
    set_page_directory(new_as->directory_phys);
}

void debug_as(as_t* as, vaddr_t addr) {
    size_t dir_idx = addr >> 22;
    size_t tab_idx = (addr >> 12) & MASK(10);
    size_t pag_idx = addr & MASK(12);

    printf("addr: %08x DIR: %d TAB: %d PAG: %d\n", addr, dir_idx, tab_idx, pag_idx);
    printf("Directory: %08x %08x\n", as->directory, as->directory_phys);
    pde_t de = as->directory->entries[dir_idx];
    printf("PDE: P: %d R/W: %d PS: %d Tab: %d\n", de.present, de.rw, de.page_size, de.page_table);
}

void vm_init() {
    vm_steal_next = pmm_get_border();
    vaddr_t managed_start = (vaddr_t) &KERNEL_BOOT_MAPPED;
    vaddr_t managed_end = 0xFF000000;
    vm_steal_remaining = managed_start - ((vaddr_t)vm_steal_next);
    DEBUG(DB_MEMORY, "[VM] Stealable: 0x%08x ... 0x%08x (%ud KiB)\n", vm_steal_next, managed_start, vm_steal_remaining >> 10);

    vm_first_as.directory = &KERNEL_PAGE_DIRECTORY;
    vm_first_as.directory_phys = virtual2phys(&KERNEL_PAGE_DIRECTORY);

    pde_t d = {.present = 1, .rw = 1};
    for (size_t i = 0x300; i < 0x400; i++) {
        as_map_kernel_table(&vm_first_as, i, d);
    }

    for (vaddr_t va = (vaddr_t) &KERNEL_OFFSET; va < (vaddr_t) &KERNEL_BOOT_MAPPED; va += PAGE_SIZE) {
        vm_map_page(&vm_first_as, virtual2phys((void*) va), va);
    }

    vm_first_bucket.next = NULL;
    vm_first_bucket.prev = NULL;
    vm_first_bucket.pageno = managed_start >> PAGE_BITS;
    vm_first_bucket.size = (managed_start - managed_end) >> PAGE_BITS;

    vm_kernel_buckets = &vm_first_bucket;

    switch_as(&vm_first_as);
}

err_t vm_reserve_pages(vm_free_bucket_t** buckets, size_t pages, size_t* pageno_out) {
    vm_free_bucket_t* head = *buckets;
    while (head) {
        if (head->size >= pages) {
            *pageno_out = head->pageno;
            if (head->size == pages) {
                // this bucket is now empty, remove it
                head->pageno = 0;
                head->size = 0;

                ll_remove_item(buckets, head);
                ll_push_item(&vm_waiting_list, head);
            } else {
                head->pageno += pages;
                head->size -= pages;
            }
            return E_OK;
        }
        head = head->next;
    }
    return E_NOMEM;
}

void vm_map_page(as_t* as, paddr_t paddr, vaddr_t vaddr) {
    size_t dir_idx = vaddr >> 22;
    size_t tab_idx = (vaddr >> 12) & 0x3FF;

    ASSERT(paddr == PAGE_ALIGN(paddr), "paddr must be page-aligned");

    pde_t* dir_entry = &as->directory->entries[dir_idx];
    if (!dir_entry->present) {
        // we need a new page table
        if (dir_idx >= 0x300) {
            PANIC("Kernel directory entry should always be present");
        } else {
            PANIC("Not implemented");
        }
    }

    // entry and table are now present
    pte_t* tab_entry = &as->tables[dir_idx]->entries[tab_idx];
    tab_entry->frame = paddr >> PAGE_BITS;
    tab_entry->present = 1;
    tab_entry->rw = 1;
}

paddr_t vm_unmap_page(vaddr_t vaddr) {
    size_t dir_idx = vaddr >> 22;
    size_t tab_idx = (vaddr >> 12) & 0x3FF;

    DEBUG(DB_MEMORY, "Unmapping page 0x%08x (DIR=%d, TAB=%d)\n", vaddr, dir_idx, tab_idx);

    pte_t* tab_entry = &current_as->tables[dir_idx]->entries[tab_idx];
    paddr_t paddr = tab_entry->frame << PAGE_BITS;
    tab_entry->present = 0;
    tab_entry->frame = 0;
    return paddr;
}

void* vm_alloc_kpages(size_t pages) {
    size_t pageno;
    err_t err;

    DEBUG(DB_MEMORY, "[VM] Allocating %d virtual pages\n", pages);

    err = vm_reserve_pages(&vm_kernel_buckets, pages, &pageno);
    ASSERT(!err, "Out of virtual memory");
    for (size_t i = 0; i < pages; i++) {
        paddr_t paddr;
        err = pmm_alloc(1, &paddr);
        ASSERT(!err, "Out of physical memory");
        vm_map_page(current_as, paddr, (pageno + i) * PAGE_SIZE);
    }

    void* ptr = (void*)(pageno * PAGE_SIZE);
    memset(ptr, 0b10101010, pages * PAGE_SIZE);
    return ptr;
}

void vm_optimize(vm_free_bucket_t** buckets, vm_free_bucket_t* item) {
    bool can_merge_front = item->prev && (item->prev->pageno + item->prev->size == item->pageno);
    bool can_merge_back = item->next && (item->pageno + item->size == item->next->pageno);
    vm_free_bucket_t* tmp;

    if (can_merge_front) {
        tmp = item->prev;
        item->pageno = tmp->pageno;
        item->size += tmp->size;
        ll_remove_item(buckets, tmp);
        ll_push_item(&vm_waiting_list, tmp);
    }

    if (can_merge_back) {
        tmp = item->next;
        item->size += tmp->size;
        ll_remove_item(buckets, tmp);
        ll_push_item(&vm_waiting_list, tmp);
    }
}

void* vm_kernel_stealmem(size_t size) {
    DEBUG(DB_MEMORY, "vm_kernel_stealmem: Stealing %d bytes\n", size);
    ASSERT(size <= PAGE_SIZE);

    if (vm_steal_remaining < size) {
        vm_steal_next = vm_alloc_kpages(1);
        vm_steal_remaining = PAGE_SIZE;
    }

    void* addr = vm_steal_next;
    memset(addr, 0, size);
    vm_steal_next += size;
    vm_steal_remaining -= size;
    return addr;
}

vm_free_bucket_t* vm_item() {
    if (vm_waiting_list) {
        return ll_pop_item(&vm_waiting_list);
    }
    // waiting list is empty
    return vm_kernel_stealmem(sizeof(vm_free_bucket_t));
}

void vm_return_pages(vm_free_bucket_t** buckets, size_t pages, size_t pageno) {
    vm_free_bucket_t* item = vm_item();
    ASSERT(item);

    item->pageno = pageno;
    item->size = pages;

    vm_free_bucket_t* node = *buckets;

    if (node) {
        while (node && node->pageno <= pageno) {
            node = node->next;
        }

        if (node) {
            // there is an item behind us
            ll_insert_before(buckets, item, node);
        } else {
            // the new item will be the last
            ll_append_notempty(item, node);
        }

        vm_optimize(buckets, item);
    } else {
        // there are no items
        ll_push_item(buckets, item);
    }
}

void vm_free_kpages(size_t pages, void* addr) {
    size_t pageno = (size_t)addr >> PAGE_BITS;
    paddr_t paddr;

    for (size_t i = 0; i < pages; i++) {
        size_t j = pages - i - 1;
        paddr = vm_unmap_page((vaddr_t)addr + j * PAGE_SIZE);
    }

    vm_return_pages(&vm_kernel_buckets, pages, pageno);
    pmm_free(pages, paddr);
}

void vm_debug() {
    vm_free_bucket_t* node = vm_kernel_buckets;
    DEBUG(DB_MEMORY, "VM DEBUG:\n");
    while (node) {
        DEBUG(DB_MEMORY, "* %08x (%08x)\n", node->pageno, node->size);
        node = node->next;
    }
}

void vm_dma(paddr_t paddr, vaddr_t vaddr, size_t size) {
    size_t pages = size / PAGE_SIZE;

    for (size_t i = 0; i < pages; i++) {
        vm_map_page(current_as, paddr + i * PAGE_SIZE, vaddr + i * PAGE_SIZE);
    }
}
