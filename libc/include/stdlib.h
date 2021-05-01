#pragma once

#include <stddef.h>

void* malloc(size_t size);
void free(void* ptr);

/// Allocate memory for an array of `nmemb` elements of `size` bytes each.
/// The memory is set to zero.
void* calloc(size_t nmemb, size_t size);
