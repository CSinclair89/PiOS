#include "mem.h"

void *memset(void *s, int c, unsigned long n) {
	unsigned char *p = s;
	while (n--) *p++ = (unsigned char)c;
	return s;
}
