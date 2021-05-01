#include "bitset.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool bitset_find(const bitset32_t* bitset, size_t* result) {
    uint32_t item;
    for (size_t i = 0; i < bitset->len; i++) {
        item = bitset->data[i];
        if (item) {
            // at least one bit in item is set, let's find it
            for (size_t j = 0; j < 32; j++) {
                if (item & 1) {
                    *result = 32 * i + j;
                    return true;
                }
                item >>= 1;
            }
        }
    }
    return false;
}

bool bitset_read(const bitset32_t* bitset, size_t idx) {
    return bitset->data[BIT_SLOT(idx)] & BIT_MASK(idx);
}

void bitset_set(bitset32_t* bitset, uint32_t idx) {
    bitset->data[BIT_SLOT(idx)] |= BIT_MASK(idx);
}

void bitset_clear(bitset32_t* bitset, uint32_t idx) {
    bitset->data[BIT_SLOT(idx)] &= ~BIT_MASK(idx);
}

void bitset_write(bitset32_t* bitset, uint32_t idx, bool state) {
    if (state) {
        bitset_set(bitset, idx);
    } else {
        bitset_clear(bitset, idx);
    }
}

void bitset_fill(bitset32_t* bitset, uint32_t data) {
    for (size_t i = 0; i < bitset->len; i++) {
        bitset->data[i] = data;
    }
}
