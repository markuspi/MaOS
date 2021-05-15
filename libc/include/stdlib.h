#pragma once

#include <stddef.h>

/// Allocate `size` bytes of memory.
/// The memory is not initialized.
extern void* malloc(size_t size);

/// Free a previously allocated region of memory.
extern void free(void* ptr);

/// Allocate memory for an array of `nmemb` elements of `size` bytes each.
/// The memory is set to zero.
extern void* calloc(size_t nmemb, size_t size);

int liballoc_lock();
int liballoc_unlock();
void* liballoc_alloc(int);
int liballoc_free(void*, int);
