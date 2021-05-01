#include "stdlib.h"

#include <stddef.h>

#include "stdio.h"
#include "string.h"

#ifdef __is_libk
#include "kernel/memory.h"

void* malloc(size_t size) {
    printf("malloc: %d bytes\n", size);
    return kmalloc(size);
}

void free(void* ptr) {
    kfree(ptr);
}
#else
// TODO
#endif

void* calloc(size_t nmemb, size_t size) {
    size_t bytes = nmemb * size;
    void* ptr = malloc(bytes);
    if (ptr == NULL) {
        return NULL;
    }
    memset(ptr, 0, bytes);
    return ptr;
}
