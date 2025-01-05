#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>
#include <types.h>

void vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);

#endif // _STDIO_H