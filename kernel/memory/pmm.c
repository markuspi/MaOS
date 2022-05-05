
#include <bitset.h>
#include <stdio.h>

#include "kernel/common.h"
#include "kernel/memory.h"
#include "kernel/linkervars.h"

/// start of managed memory region, page-aligned
static paddr_t managed_first;
/// end of managed memory region, page-aligned
static paddr_t managed_last;

#define PMM_NUM_BUDDIES 7

/**
 * First buddy has a bit for each frame,
 * Second buddy for every 2nd frame, and so on
 */

static bitset32_t buddies[PMM_NUM_BUDDIES];
static paddr_t buddy_base;

static size_t pmm_estimate_bytes_required(size_t avail) {
    size_t n_frames = avail / PAGE_SIZE;
    // each frame takes 2 bits -> 4 frames per byte
    size_t bytes_required = n_frames / 4;
    return bytes_required;
}

void* pmm_phys2virtual(paddr_t addr) {
    return (void*) (addr + (paddr_t) &KERNEL_OFFSET);
}

void* pmm_get_border() {
    return pmm_phys2virtual(managed_first);
}

void pmm_init(paddr_t upper_bound) {
    // this function will be called with boot pse page mapping,
    // so we have 4 MiB to play with

    paddr_t avail_first = (paddr_t) &KERNEL_END_PHYS;
    managed_last = upper_bound;

    size_t avail = managed_last - avail_first;
    size_t required = pmm_estimate_bytes_required(avail);

    DEBUG(DB_MEMORY, "[PMM] Available RAM: %ud KiB (%ud MiB)\n", avail >> 10, avail >> 20);
    DEBUG(DB_MEMORY, "[PMM] Requires %ud bytes for buddies\n", required);

    managed_first = PAGE_ALIGN(avail_first + required);
    size_t managed = managed_last - managed_first;

    // ensure that this region is still mapped
    ASSERT(managed_first <= (paddr_t) &KERNEL_BOOT_MAPPED);

    DEBUG(DB_MEMORY, "[PMM] Buddies live in 0x%08x ... 0x%08x\n", avail_first, managed_first);

    size_t n_frames = ALIGN_BITS_DOWN(managed / PAGE_SIZE, PMM_NUM_BUDDIES - 1 + 5);

    // [avail_first; managed_first - 1] is now exclusively for our buddies
    buddies[0].data = pmm_phys2virtual(avail_first);
    buddies[0].len = n_frames / 32; // 32 bits per dword

    // initialize lower-level buddies
    for (size_t i = 1; i < PMM_NUM_BUDDIES; i++) {
        buddies[i].len = n_frames >> (5 + i);
        buddies[i].data = buddies[i - 1].data + buddies[i - 1].len;
    }

    for (size_t i = 0; i < PMM_NUM_BUDDIES - 1; i++) {
        bitset_fill(&buddies[i], 0);
    }
    bitset_fill(&buddies[PMM_NUM_BUDDIES - 1], 0xFFFFFFFF);

    buddy_base = managed_first;

    DEBUG(DB_MEMORY, "[PMM] RAM Bootstrapped. Usable Memory: 0x%08x ... 0x%08x\n", managed_first, managed_last);
    DEBUG(DB_MEMORY, "[PMM] %ud KiB (%ud MiB) of memory managed (%ud frames)\n", managed >> 10, managed >> 20, n_frames);
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
        mark_used(level - 1, n_frames > capacity ? n_frames - capacity : 0, idx * 2 + 1);
    }
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
            DEBUG(DB_MEMORY, "[PMM] Buddy allocated %d frame(s): %d %d -> 0x%08x\n", n_frames, i, idx, *addr);
            return E_OK;
        }
    }

    *addr = 0;
    return E_NOMEM;
}

err_t pmm_alloc(size_t n_frames, paddr_t* addr) {
    return pmm_alloc_buddy(n_frames, addr);
}

void pmm_free(size_t n_frames, paddr_t addr) {
    if (addr < buddy_base) {
        PANIC("Cannot free stolen pages");
    }

    DEBUG(DB_MEMORY, "PMM FREE 0x%08x\n", addr);

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
