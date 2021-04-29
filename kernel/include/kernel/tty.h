#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    TTY_COLOR_BLACK = 0,
	TTY_COLOR_BLUE = 1,
	TTY_COLOR_GREEN = 2,
	TTY_COLOR_CYAN = 3,
	TTY_COLOR_RED = 4,
	TTY_COLOR_MAGENTA = 5,
	TTY_COLOR_BROWN = 6,
	TTY_COLOR_LIGHT_GREY = 7,
	TTY_COLOR_DARK_GREY = 8,
	TTY_COLOR_LIGHT_BLUE = 9,
	TTY_COLOR_LIGHT_GREEN = 10,
	TTY_COLOR_LIGHT_CYAN = 11,
	TTY_COLOR_LIGHT_RED = 12,
	TTY_COLOR_LIGHT_MAGENTA = 13,
	TTY_COLOR_LIGHT_BROWN = 14,
	TTY_COLOR_WHITE = 15,
} tty_color_t;


uint8_t tty_makecolor(tty_color_t fg, tty_color_t bg);

void tty_putcharat(unsigned char c, uint8_t color, size_t x, size_t y);
void tty_putchar(char c);
void tty_writebuf(const char* buf, size_t size);
void tty_writestr(const char* str);
void tty_setcolor(uint8_t color);
void tty_clear();
void tty_init();
uint16_t tty_entry(unsigned char c, uint8_t color);
