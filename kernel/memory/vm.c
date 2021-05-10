
#include "kernel/vm.h"

#include <string.h>

#include "kernel/common.h"
#include "kernel/memory.h"

extern char _kernel_end, _kernel_start;
extern char phys_page_directory, phys_page_table, boot_page_table;
extern char phys_special_table;
extern page_table_t boot_special_table;
extern page_directory_t boot_page_directory;

static vm_free_bucket_t* vm_kernel_buckets;
static vm_free_bucket_t* vm_waiting_list;
static void* vm_steal_next = NULL;
static size_t vm_steal_remaining = 0;
static vaddr_t vm_ktable_offset;

static vm_free_bucket_t vm_first_bucket;
static as_t vm_first_as;
static size_t vm_magic_idx;

as_t* current_as = NULL;

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

void vm_init() {
    vaddr_t kernel_start = 0xC0100000;
    vaddr_t kernel_end = (vaddr_t)&_kernel_end;

    vaddr_t region_end = kernel_end + pmm_estimate_bytes_required();

    vm_ktable_offset = ALIGN_BITS_UP(region_end, 22);  // 4 MiB aligned
    vaddr_t managed_start = vm_ktable_offset + 4 * MiB;
    vaddr_t managed_end = 0xFF000000;

    vm_first_as.directory = &boot_page_directory;
    vm_first_as.directory_phys = (paddr_t)&phys_page_directory;

    vm_magic_idx = vm_ktable_offset >> 22;

    boot_special_table.entries[vm_magic_idx].present = 1;
    boot_special_table.entries[vm_magic_idx].rw = 1;
    boot_special_table.entries[vm_magic_idx].frame = ((paddr_t)&phys_special_table) >> PAGE_BITS;

    // manually map the table that manages our ktables
    pde_t* dir_entry = &vm_first_as.directory->entries[vm_magic_idx];
    dir_entry->present = 1;
    dir_entry->rw = 1;
    dir_entry->page_table = ((paddr_t)&phys_special_table) >> PAGE_BITS;

    // tables[idx] is 4 KiB large, we must map that page beforehand, as it is a fixed point
    vm_first_as.tables[vm_magic_idx] = (page_table_t*)(vm_ktable_offset + vm_magic_idx * PAGE_SIZE);
    printf("Manually mapped %08x -> %08x\n", vm_ktable_offset + vm_magic_idx * PAGE_SIZE,
           &phys_special_table);

    vm_first_as.tables[kernel_start >> 22] = (page_table_t*)&boot_page_table;
    boot_special_table.entries[kernel_start >> 22].present = 1;
    boot_special_table.entries[kernel_start >> 22].rw = 1;
    boot_special_table.entries[kernel_start >> 22].frame = ((paddr_t)&phys_page_table) >> PAGE_BITS;

    current_as = &vm_first_as;

    // now we need to map [kernel_start; vm_ktable_offset)
    for (size_t v = kernel_start; v < vm_ktable_offset; v += PAGE_SIZE) {
        vm_map_page(v - 0xC0000000, v);
    }

    DEBUG(DB_MEMORY,
          "VM initialized.\nKernel: %x - %x, Region End: %x, KTable: %x, Managed: %x - %x\n",
          kernel_start, kernel_end, region_end, vm_ktable_offset, managed_start, managed_end);

    vm_first_bucket.next = NULL;
    vm_first_bucket.prev = NULL;
    vm_first_bucket.pageno = managed_start >> PAGE_BITS;
    vm_first_bucket.size = (managed_start - managed_end) >> PAGE_BITS;

    vm_kernel_buckets = &vm_first_bucket;
    pmm_set_init_memory((void*)kernel_end);
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

void vm_map_page(paddr_t paddr, vaddr_t vaddr) {
    size_t dir_idx = vaddr >> 22;
    size_t tab_idx = (vaddr >> 12) & 0x3FF;

    if (dir_idx == vm_magic_idx && tab_idx == vm_magic_idx) return;

    printf("Mapping: %x -> %x (DIR %d, TAB %d)\n", vaddr, paddr, dir_idx, tab_idx);

    pde_t* dir_entry = &current_as->directory->entries[dir_idx];
    if (!dir_entry->present) {
        // we need a new page table
        if (dir_idx >= 0x300) {
            DEBUG(DB_MEMORY, "Creating new kpage table\n");
            // special treatment for kernel pages
            // we have a dedicated virtual memory region for kernel page tables
            vaddr_t table_vaddr = vm_ktable_offset + dir_idx * PAGE_SIZE;
            paddr_t table_paddr;
            pmm_alloc(1, &table_paddr);
            vm_map_page(table_paddr, table_vaddr);
            // table is now mapped and safe to use
            page_table_t* table_ptr = (page_table_t*)table_vaddr;

            dir_entry->present = 1;
            dir_entry->rw = 1;
            dir_entry->page_table = table_paddr >> PAGE_BITS;

            current_as->tables[dir_idx] = table_ptr;
            memset(table_ptr, 0, PAGE_SIZE);
        } else {
            PANIC("Not implemented");
        }
    }

    // entry and table are now present
    pte_t* tab_entry = &current_as->tables[dir_idx]->entries[tab_idx];
    if (dir_idx == 769 && tab_idx == 770) {
        printf("Table entry is at virt 0x%08x and phys 0x%08x\n", tab_entry,
               current_as->directory->entries[dir_idx].page_table * PAGE_SIZE + 4 * tab_idx);
    }
    tab_entry->frame = paddr >> PAGE_BITS;
    tab_entry->present = 1;
    tab_entry->rw = 1;
}

paddr_t vm_unmap_page(vaddr_t vaddr) {
    size_t dir_idx = vaddr >> 22;
    size_t tab_idx = (vaddr >> 12) & 0x3FF;

    pte_t* tab_entry = &current_as->tables[dir_idx]->entries[tab_idx];
    paddr_t paddr = tab_entry->frame << PAGE_BITS;
    tab_entry->present = 0;
    tab_entry->frame = 0;
    return paddr;
}

void* vm_alloc_kpages(size_t pages) {
    paddr_t paddr;
    size_t pageno;
    err_t err;

    err = pmm_alloc(pages, &paddr);
    ASSERT(!err, "Out of physical memory");
    err = vm_reserve_pages(&vm_kernel_buckets, pages, &pageno);
    ASSERT(!err, "Out of virtual memory");
    for (size_t i = 0; i < pages; i++) {
        vm_map_page(paddr + i * PAGE_SIZE, (pageno + i) * PAGE_SIZE);
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
    size_t pageno = (size_t)pageno >> PAGE_BITS;
    paddr_t paddr;

    for (size_t i = pages - 1; i >= 0; i--) {
        paddr = vm_unmap_page((vaddr_t)addr + i * PAGE_SIZE);
    }

    vm_return_pages(&vm_kernel_buckets, pages, pageno);
    pmm_free(pages, paddr);
}
