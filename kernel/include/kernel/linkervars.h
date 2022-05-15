/*
 * This file contains variables that are defined by the linker script.
 */
#pragma once

#include "common.h"

CPP_GUARD_START

// defined in linker.ld
extern char KERNEL_OFFSET;
extern char KERNEL_START;
extern char KERNEL_START_PHYS;
extern char KERNEL_END;
extern char KERNEL_END_PHYS;
extern char KERNEL_BOOT_MAPPED;

CPP_GUARD_END
