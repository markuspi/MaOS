#pragma once

#include <stdint.h>

#include "kernel/common.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Pushed by pusha
    uint32_t int_no, err_code;  // interrupt number, error code
} PACKED registers_t;

typedef void (* isr_t)(uint8_t);
