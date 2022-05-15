#pragma once

#include <stdint.h>

#include "kernel/common.h"

CPP_GUARD_START

typedef struct {
    uint32_t dummy1, dummy2; // values that are pushed by the isr handler
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
    uint32_t int_no, err_code;  // interrupt number, error code
    uint32_t eip;
} PACKED registers_t;

typedef enum {
    IRQ0 = 32, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6
} interrupt_t;

typedef void (*isr_t)(uint8_t);

void irq_register_handler(interrupt_t int_no, isr_t handler);

CPP_GUARD_END
