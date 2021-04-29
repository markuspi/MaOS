#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "stdio.h"

#define try(var, expr) if ((var = (expr)) < 0) { return var; }

static const char* hexchars = "0123456789ABCDEF";

static int print(const char* buf, size_t len)
{
    int error;

    for (size_t i = 0; i < len; i++) {
        try(error, putchar(buf[i]));
    }
    
    return (int) len;
}

static int print_base(int32_t num, int base, bool sig, int pad_len, char pad_char)
{
    char buffer[15];
    uint32_t unum;
    int error;

    if (sig && num < 0) {
        try(error, putchar('-'));
        pad_len--;
        unum = (uint32_t) -num;
    } else {
        unum = (uint32_t) num;
    }

    int i;
    for (i = 0; i < 15; i++) {
        buffer[i] = (char) hexchars[unum % base];
        if (unum < base) break;
        unum /= base;
    }

    while (--pad_len > i) {
        try(error, putchar(pad_char));
    }    
    
    for (int j = i; j >= 0; j--) {
        putchar(buffer[j]);
    }    
}

int printf(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    size_t written = 0;
    int error;

    while (*format != '\0')
    {
        if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%') {
				amount++;
            }
            try(error, print(format, amount));
			format += amount;
			written += amount;
			continue;
		}

        format++;
        bool sig = true;
        int pad_len = 0;
        char pad_char = ' ';

        if (*format == '0') {
            pad_char = '0';
            format++;
        }

        while (*format >= '0' && *format <= '9') {
            int digit = *format - '0';
            pad_len = (pad_len * 10) + digit;
            format++;
        }        

        if (*format == 'u') {
            format++;
            sig = false;
        }

        if (*format == 's') {
            format++;
            const char* str = va_arg(args, const char*);
            size_t len = strlen(str);
            while (pad_len > len)
            {
                try(error, putchar(pad_char));
                pad_len--;
            }            
            try(error, print(str, len));
            written += len;
        } else if (*format == 'd') {
            format++;
            int32_t num = va_arg(args, int32_t);
            print_base(num, 10, sig, pad_len, pad_char);
        } else if (*format == 'x') {
            format++;
            int32_t num = va_arg(args, int32_t);
            print_base(num, 16, false, pad_len, pad_char);
        }
    }    

    va_end(args);
    return written;
}