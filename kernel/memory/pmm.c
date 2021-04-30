
#include <stdint.h>
#include <stdio.h>

#include "kernel/common.h"

#include "kernel/memory.h"

extern char _kernel_end_phys;

/// start of managed memory region, page-aligned
static paddr_t managed_first;
/// end of managed memory region, page-aligned
static paddr_t managed_last;
/// first free address in stealing phase
static paddr_t free_first;

static pmm_state_t pmm_state = pmm_state_uninitialized;

#define PMM_NUM_BUDDIES 7

static uint32_t* buddies[PMM_NUM_BUDDIES];


void pmm_bootstrap()
{
    managed_first = PAGE_ALIGN((uint32_t) &_kernel_end_phys);
    managed_last = 130 * 1024 * 1024;  // TODO: use actual value from bootloader

    free_first = managed_first;

    uint32_t avail = managed_last - managed_first;

    printf("RAM Bootstrapped. Usable Memory: 0x%08x ... 0x%08x\n", managed_first, managed_last);
    printf("%ud KiB (%ud MiB) of memory managed\n", avail >> 10, avail >> 20);

    pmm_state = pmm_state_steal;
}

void pmm_init()
{
    uint32_t avail = managed_last - free_first;
    size_t n_frames = avail / PAGE_SIZE;
    size_t bytes_required = n_frames / 4;

    buddies[0] = kmalloc(bytes_required);
}

static err_t pmm_steal(size_t n_frames, paddr_t* addr)
{
    size_t size = n_frames * PAGE_SIZE;

    if (free_first + size > managed_last) {
        return E_NOMEM;
    }

    *addr = free_first;
    free_first += size;

    printf("Stole %d frames at 0x%08x\n", n_frames, *addr);

    return E_OK;
}

err_t pmm_alloc(size_t n_frames, paddr_t* addr)
{
    switch (pmm_state)
    {
    case pmm_state_buddy:
        PANIC("State buddy not implemented");
        break;
    case pmm_state_steal:
        return pmm_steal(n_frames, addr);
        break;
    default:
        PANIC("Invalid pmm state %d", pmm_state);
        break;
    }
}
