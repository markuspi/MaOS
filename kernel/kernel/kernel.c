#include <stdint.h>
#include <stddef.h>

typedef enum {
    VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
} vga_color_t;

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_column;
size_t terminal_row;
uint8_t terminal_color;
uint16_t* terminal_buffer;

static uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

static uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg)
{
    return fg | bg << 4;
}

void terminal_init()
{
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
        
    }
    
}

void terminal_newline()
{
    terminal_column = 0;
    if (++terminal_row == VGA_HEIGHT)
    {
        terminal_row = 0;
    }
}

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

void terminal_put_char(const char c)
{
    size_t idx;

    switch (c)
    {
    case '\n':
        terminal_newline();
        break;
    default:
        idx = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[idx] = vga_entry(c, terminal_color);
        if (++terminal_column == VGA_WIDTH)
        {
            terminal_newline();
        }
        break;
    }
}

void terminal_write_buf(const char* buffer, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        terminal_put_char(buffer[i]);
    }   
}

void terminal_write_str(const char* data)
{
    terminal_write_buf(data, strlen(data));
}

void kernel_main()
{
    terminal_init();
    terminal_write_str("Hello, World!\n");
    terminal_write_str("Nice\n");
}
