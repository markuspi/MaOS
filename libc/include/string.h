#pragma once

#include <stddef.h>
#include <utils.h>

CPP_GUARD_START

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void memcpy_aligned(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);

/** Fills the first n bytes of the memory area
 * pointed to by s with the constant byte c */
void* memset(void* s, int c, size_t n);

size_t strlen(const char* str);
char* strdup(const char* s);

CPP_GUARD_END
