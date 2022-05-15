#pragma once

#include <types.h>
#include <utils.h>

CPP_GUARD_START

#define BIT_MASK(b) (1 << ((b) % 32))
#define BIT_SLOT(b) ((b) / 32)

typedef struct {
    /// underlying data
    uint32_t* data;

    /// number of dwords
    size_t len;
} bitset32_t;

/// find the index of the first set entry
bool bitset_find(const bitset32_t* bitset, size_t* result);

void bitset_set(bitset32_t* bitset, uint32_t idx);

void bitset_clear(bitset32_t* bitset, uint32_t idx);

void bitset_write(bitset32_t* bitset, uint32_t idx, bool state);

bool bitset_read(const bitset32_t* bitset, uint32_t idx);

void bitset_fill(bitset32_t* bitset, uint32_t data);

CPP_GUARD_END
