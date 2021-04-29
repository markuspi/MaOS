
#include "stdio.h"

int puts(const char* str)
{
    char c;
    while (c = *str++)
    {
        putchar(c);
    }
}
