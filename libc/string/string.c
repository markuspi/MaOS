
#include <stddef.h>

#include "string.h"

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memset(void* bufptr, int value, size_t n) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < n; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

