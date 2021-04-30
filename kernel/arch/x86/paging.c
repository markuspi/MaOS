
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "kernel/common.h"

#include "kernel/memory.h"

extern char _kernel_end;
extern char KERNEL_OFFSET;

extern uint32_t boot_page_table[];
extern uint32_t boot_page_table_end[];
extern uint32_t boot_page_directory[];
extern uint32_t boot_used_pages;

static uint32_t boot_mapped_pages;
static uint32_t next_addr;

void paging_bootstrap()
{
    boot_mapped_pages = (boot_page_table_end - boot_page_table);
    printf("Boot Mapped Pages: %d Used Pages: %d\n", boot_mapped_pages, boot_used_pages);
}

void paging_init()
{

}

void* paging_steal(size_t n_pages, paddr_t* phys)
{
    paddr_t paddr;
    err_t error;

    error = pmm_alloc(n_pages, &paddr);
    ASSERT(!error, "Could not allocate memory");

    if (boot_used_pages + n_pages > boot_mapped_pages) {
        PANIC("OUT OF BOOT MEMORY");
    }

    uint32_t vaddr = (uint32_t) &KERNEL_OFFSET + (boot_used_pages) * PAGE_SIZE;
    
    for (size_t i = 0; i < n_pages; i++)
    {
        boot_page_table[boot_used_pages] = paddr + (i * PAGE_SIZE) + 0b11;
        boot_used_pages++;
    }    

    printf("Stole %d page(s): 0x%08x -> 0x%08x. %d left\n", n_pages, vaddr, paddr, boot_mapped_pages - boot_used_pages);

    if (phys != NULL) {
        *phys = paddr;
    }

    return (void*) vaddr;
}
