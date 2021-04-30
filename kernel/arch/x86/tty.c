
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernel/tty.h"

static const size_t TTY_WIDTH = 80;
static const size_t TTY_HEIGHT = 25;

static size_t tty_row;
static size_t tty_column;
static uint16_t* tty_buf = (uint16_t*) 0xC00B8000;
static uint8_t tty_color;

static void newline()
{
    if (++tty_row == TTY_HEIGHT)
    {
        tty_row = 0;
    }

    // clear line
    for (size_t i = 0; i < TTY_WIDTH; i++)
    {
        tty_putcharat(' ', 0, i, tty_row);
    }    
}

void tty_init()
{
    tty_color = tty_makecolor(TTY_COLOR_WHITE, TTY_COLOR_BLACK);
    tty_clear();
}

void tty_clear()
{
    memset(tty_buf, '\x00', TTY_WIDTH * TTY_HEIGHT * 2);
}

uint8_t tty_makecolor(tty_color_t fg, tty_color_t bg)
{
    return fg | bg << 4;
}

void tty_putchar(char c)
{
    unsigned char uc = c;

    switch (c)
    {
    case '\n':
        newline();
        /* falls through */
    case '\r':
        tty_column = 0;
        return;
    default:
        break;
    }

    tty_putcharat(uc, tty_color, tty_column, tty_row);
    if (++tty_column == TTY_WIDTH)
    {
        tty_column = 0;
        newline();
    }
}

void tty_putcharat(unsigned char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * TTY_WIDTH + x;
    tty_buf[index] = tty_entry(c, color);
}

void tty_writebuf(const char* buf, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        tty_putchar(buf[i]);
    }    
}

void tty_writestr(const char* str)
{
    char x;
    for (size_t i = 0; x = str[i]; i++)
    {
        tty_putchar(x);
    }       
}

uint16_t tty_entry(unsigned char c, uint8_t color)
{
    return (uint16_t) c | (uint16_t) color << 8;
}

void tty_setcolor(uint8_t color)
{
    tty_color = color;
}
