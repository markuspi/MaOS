#pragma once

#define PACKED __attribute__((packed))
#define PANIC(format, args...) kernel_panic(__FILE__, __LINE__, format, args)
#define ASSERT(exp, args...)  if (!(exp)) PANIC("Assertion Error: " args)

void halt();

void kernel_panic(const char* filename, int line, const char* format, ...);
