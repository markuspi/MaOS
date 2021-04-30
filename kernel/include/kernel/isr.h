#pragma once

#include <stdint.h>

#include "kernel/common.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
    uint32_t int_no, err_code;  // interrupt number, error code
    uint32_t eip;
} PACKED registers_t;

typedef void (* isr_t)(uint8_t);

void irq_register_handler(uint8_t int_no, isr_t handler);
