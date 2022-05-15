#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <utils.h>

CPP_GUARD_START

typedef enum { E_OK = 0, E_NOMEM = -1, E_DEVICE = -2 } err_t;

#define KiB 1024
#define MiB 0x100000

#define DB_MEMORY 0x0001
#define DB_PCI 0x0002
#define DB_GFX 0x0004

#define PACKED __attribute__((packed))
#define UNUSED __attribute__((unused))

#define PANIC(format, ...) kernel_panic(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define ASSERT(exp, args...) (exp ? 0 : PANIC("Assertion Error: (" #exp ") == false\n" args))
#define DEBUG(d, ...) ((dbflags & (d)) ? printf(__VA_ARGS__) : 0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/// returns n binary 1s, e.g. MASK(4) == 0b1111
#define MASK(bits) ((1 << (bits)) - 1)

#define ALIGN_BITS_DOWN(val, bits) ((val) & ~MASK(bits))
#define ALIGN_BITS_UP(val, bits) (((val) + MASK(bits)) & ~(MASK(bits)))

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

void halt();

/// enable interrupts
void sti();

/// disable interrupts
void cli();

/// NOP operation, do nothing
void nop();

/// `n` NOP operations
void nopN(size_t n);

void kernel_panic(const char* filename, int line, const char* format, ...);

extern uint32_t dbflags;

CPP_GUARD_END
