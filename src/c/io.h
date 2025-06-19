#ifndef IO_H
#define IO_H

#include <stdarg.h>

typedef long unsigned int size_t;
typedef char *charptr;
typedef int (*func_ptr)(int c);

void putu(unsigned int x);
void puthex(unsigned long long x);

void printp(charptr str, ...);
void printv(charptr str, ...);
void vprintf(const func_ptr f_ptr, charptr ctrl, va_list argp);

#endif
