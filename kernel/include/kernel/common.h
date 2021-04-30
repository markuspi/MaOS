#pragma once

#include <stdint.h>

typedef enum {
    E_OK = 0,
    E_NOMEM = -1
} err_t;

#define PACKED __attribute__((packed))
#define PANIC(format, ...) kernel_panic(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define ASSERT(exp, args...)  if (!(exp)) PANIC("Assertion Error: (" #exp ") == false\n" args)

typedef uint32_t paddr_t;

void halt();

void kernel_panic(const char* filename, int line, const char* format, ...);
