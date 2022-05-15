#pragma once

#include <utils.h>

#include <stdarg.h>
#include <stddef.h>

CPP_GUARD_START

#define EOF -1

int vprintf(const char* format, va_list args);
int printf(const char* format, ...);
int putchar(int c);
int puts(const char* str);

CPP_GUARD_END
