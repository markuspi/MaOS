#pragma once

#define PACKED __attribute__((packed))
#define PANIC(format, ...) kernel_panic(__FILE__, __LINE__, format, ##__VA_ARGS__)
#define ASSERT(exp, args...)  if (!(exp)) PANIC("Assertion Error: (" #exp ") == false\n" args)

void halt();

void kernel_panic(const char* filename, int line, const char* format, ...);
