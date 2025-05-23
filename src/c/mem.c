#include "mem.h"

void *memset(void *s, int c, unsigned long n) {
	unsigned char *p = s;
	while (n--) *p++ = (unsigned char)c;
	return s;
}

void *memcpy(void *dest, const void *src, unsigned long size) {
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	for (unsigned long i = 0; i < size; i++) d[i] = s[i];
	return dest;
}
