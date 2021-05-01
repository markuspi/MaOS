
#include <bitset.h>
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

/**
 * First buddy has a bit for each frame,
 * Second buddy for every 2nd frame, and so on
 */

static bitset32_t buddies[PMM_NUM_BUDDIES];

static size_t buddy_len;
static paddr_t buddy_base;

void pmm_bootstrap() {
    managed_first = PAGE_ALIGN((uint32_t)&_kernel_end_phys);
    managed_last = 130 * 1024 * 1024;  // TODO: use actual value from bootloader

    free_first = managed_first;

    uint32_t avail = managed_last - managed_first;

    printf("RAM Bootstrapped. Usable Memory: 0x%08x ... 0x%08x\n",
           managed_first, managed_last);
    printf("%ud KiB (%ud MiB) of memory managed\n", avail >> 10, avail >> 20);

    pmm_state = pmm_state_steal;
}

void pmm_init() {
    uint32_t avail = managed_last - free_first;
    size_t n_frames = avail / PAGE_SIZE;
    size_t bytes_required = n_frames / 4;

    void* mem = kmalloc(bytes_required);

    // kmalloc probably stole some pages, so recalculate memory
    avail = managed_last - free_first;
    n_frames = ALIGN_BITS_DOWN(avail / PAGE_SIZE, PMM_NUM_BUDDIES - 1 + 5);

    buddies[0].data = mem;
    buddies[0].len = n_frames >> 5;

    for (size_t i = 1; i < PMM_NUM_BUDDIES; i++) {
        buddies[i].len = n_frames >> (5 + i);
        buddies[i].data = buddies[i - 1].data + buddies[i - 1].len;
    }

    for (size_t i = 0; i < PMM_NUM_BUDDIES; i++) {
        bitset_fill(&buddies[i], (i == PMM_NUM_BUDDIES - 1) ? 0xFFFFFFFF : 0);
    }

    buddy_base = free_first;

    printf("PMM initialized. First buddy len: 0x%x, last buddy len: 0x%x\n",
           buddies[0].len, buddies[PMM_NUM_BUDDIES - 1].len);
    printf("Number of managed frames: 0x%x (%d KiB = %d MiB)\n", n_frames,
           n_frames * 4, (n_frames * 4) >> 10);

    pmm_state = pmm_state_buddy;
}

static void mark_used(int level, size_t n_frames, size_t idx) {
    size_t capacity = 1 << level;
    if (n_frames == capacity) {
        // we fill it exactly
        bitset_clear(&buddies[level], idx);
    } else if (n_frames == 0) {
        bitset_set(&buddies[level], idx);
    } else {
        capacity /= 2;
        mark_used(level - 1, MIN(n_frames, capacity), idx * 2);
        mark_used(level - 1, n_frames > capacity ? n_frames - capacity : 0,
                  idx * 2 + 1);
    }
}

static err_t pmm_alloc_steal(size_t n_frames, paddr_t* addr) {
    size_t size = n_frames * PAGE_SIZE;

    if (free_first + size > managed_last) {
        return E_NOMEM;
    }

    *addr = free_first;
    free_first += size;

    printf("Stole %d frames at 0x%08x\n", n_frames, *addr);

    return E_OK;
}

/* https://stackoverflow.com/a/9612244 */
static int log2(size_t n) {
    int ans = 0;
    while (n >>= 1) ans++;
    return ans;
}

static err_t pmm_alloc_buddy(size_t n_frames, paddr_t* addr) {
    int min_level = log2(n_frames);

    // TODO handle case when n_frames is large

    size_t idx;
    for (size_t i = min_level; i < PMM_NUM_BUDDIES; i++) {
        if (bitset_find(&buddies[i], &idx)) {
            bitset_clear(&buddies[i], idx);
            mark_used(i, n_frames, idx);

            *addr = buddy_base + (PAGE_SIZE << i) * idx;
            printf("Buddy allocated %d frame(s): %d %d -> 0x%08x\n", n_frames,
                   i, idx, *addr);
            return E_OK;
        }
    }

    *addr = 0;
    return E_NOMEM;
}

err_t pmm_alloc(size_t n_frames, paddr_t* addr) {
    switch (pmm_state) {
        case pmm_state_buddy:
            return pmm_alloc_buddy(n_frames, addr);
        case pmm_state_steal:
            return pmm_alloc_steal(n_frames, addr);
        default:
            PANIC("Invalid pmm state %d", pmm_state);
            break;
    }
}

void pmm_free(size_t n_frames, paddr_t addr) {
    if (addr < buddy_base) {
        PANIC("Cannot free stolen pages");
    }

    size_t frame_no = (addr - buddy_base) / PAGE_SIZE;
    int level = log2(n_frames);
    size_t idx = frame_no >> level;

    bitset_set(&buddies[level], idx);

    // TODO recombine
}

void pmm_debug() {
    for (int i = 0; i < PMM_NUM_BUDDIES; ++i) {
        size_t spacing = (1 << i) - 1;
        printf("%d: ", i);
        for (int j = 0; j < (1 << (6 - i)); ++j) {
            printf("%s", bitset_read(&buddies[i], j) ? "#" : ".");
            for (int k = 0; k < spacing; ++k) {
                printf(" ");
            }
        }
        printf("\n");
    }
}
