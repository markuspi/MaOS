#pragma once

#include <stdarg.h>
#include <stddef.h>

#define EOF -1

int vprintf(const char* format, va_list args);
int printf(const char* format, ...);
int putchar(int c);
int puts(const char* str);
