#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    E_OK = 0,
    E_NOMEM = -1
} err_t;

#define PACKED __attribute__((packed))
#define PANIC(format, ...) kernel_panic(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define ASSERT(exp, args...)  if (!(exp)) PANIC("Assertion Error: (" #exp ") == false\n" args)

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define ALIGN_BITS_DOWN(val, bits) ((val) & ~((1 << (bits)) - 1))

typedef uint32_t paddr_t;

void halt();
void sti();
void nop();
void nopN(size_t n);

void kernel_panic(const char* filename, int line, const char* format, ...);
