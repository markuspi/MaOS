#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stdio.h"

#define printf_try(expr)    \
    {                       \
        int error = (expr); \
        if (error < 0) {    \
            return error;   \
        }                   \
    } do {} while(false)

static const char* hexchars = "0123456789ABCDEF";

static int print(const char* buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf_try(putchar(buf[i]));
    }

    return (int)len;
}

static int print_int(int32_t num, int base, bool sig, int width, char pad_char) {
    char buffer[15]; // buffer[0] contains right-most char!
    uint32_t unum;

    if (sig && num < 0) {
        printf_try(putchar('-'));
        width--;
        unum = (uint32_t)-num;
    } else {
        unum = (uint32_t)num;
    }

    int i;
    for (i = 0; i < 15; i++) {
        buffer[i] = hexchars[unum % base];
        if (unum < base) break;
        unum /= base;
    }

    while (--width > i) {
        printf_try(putchar(pad_char));
    }

    for (int j = i; j >= 0; j--) {
        putchar(buffer[j]);
    }
}

static int print_double(double num, int width, int precision, char pad_char) {
    if (precision > 10) {
        precision = 10;
    }

    if (num < 0) {
        printf_try(putchar('-'));
        width--;
        num = -num;
    }

    // print integer part
    print_int((int32_t) num, 10, false, width - precision - 1, pad_char);

    putchar('.');

    // print decimal part
    double num_shifted = num;
    for (int i = 1; i <= precision; i++) {
        num_shifted *= 10;
        int num_int = (int) num_shifted;
        int digit = num_int % 10;

        // rounding of last digit
        if (i == precision && num_shifted - num_int > 0.5 - 1e-9) {
            digit++;
        }

        putchar(hexchars[digit]);
    }
}

/// parses an unsigned integer and advances the char pointer
static int parse_uint(const char** str) {
    const char* x = *str;
    int n = 0;
    while (*x >= '0' && *x <= '9') {
        int digit = *x - '0';
        n = (n * 10) + digit;
        x++;
    }
    *str = x;
    return n;
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    int result = vprintf(format, args);

    va_end(args);
    return result;
}

int vprintf(const char* format, va_list args) {
    size_t written = 0;

    while (*format != '\0') {
        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%') {
                amount++;
            }
            printf_try(print(format, amount));
            format += amount;
            written += amount;
            continue;
        }

        format++;
        bool sig = true;
        bool large = false;
        char pad_char = ' ';

        if (*format == '0') {
            pad_char = '0';
            format++;
        }

        int width = parse_uint(&format);

        int precision = 6;
        if (*format == '.') {
            format++;
            precision = parse_uint(&format);
        }

        if (*format == 'u') {
            format++;
            sig = false;
        }

        if (*format == 'l') {
            format++;
            large = true;
        }

        char specifier = *format++;
        switch (specifier) {
            case 's': {
                const char* str = va_arg(args, const char*);
                size_t len = strlen(str);
                while (width > len) {
                    printf_try(putchar(pad_char));
                    width--;
                }
                printf_try(print(str, len));
                written += len;
                break;
            }
            case 'd': {
                int32_t num = va_arg(args, int32_t);
                print_int(num, 10, sig, width, pad_char);
                break;
            }
            case 'x': {
                int32_t num = va_arg(args, int32_t);
                if (large) {
                    int32_t high = va_arg(args, int32_t);
                    print_int(high, 16, false, width - 8, pad_char);
                }
                print_int(num, 16, false, width, pad_char);
                break;
            }
            case 'b': {
                int32_t num = va_arg(args, int32_t);
                print_int(num, 2, false, width, pad_char);
                break;
            }
            case 'f': {
                double num = va_arg(args, double);
                print_double(num, width, precision, pad_char);
                break;
            }
            default: {
                printf("printf: unknown format specifier %c");
                break;
            }
        }
    }

    return (int) written;
}
