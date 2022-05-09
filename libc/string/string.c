
#include "string.h"

#include <stddef.h>

#include "stdlib.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strdup(const char* s) {
    size_t len = strlen(s);
    char* dest = malloc(len + 1);
    if (dest == NULL) return NULL;
    return memcpy(dest, s, len + 1);
}

void* memset(void* bufptr, int value, size_t n) {
    unsigned char* buf = (unsigned char*)bufptr;
    for (size_t i = 0; i < n; i++) {
        buf[i] = (unsigned char)value;
    }
    return bufptr;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    char* d = dest;
    const char* s = src;
    if (n < 16) {
        for (; n > 0; n--, d++, s++) {
            *d = *s;
        }
        return dest;
    }

    // copies multiple of 4 bytes, might miss last < 4 bytes
    size_t n_aligned = n & 0xFFFFFFFC;
    memcpy_aligned(dest, src, n_aligned);

    for (size_t i = n_aligned; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}
