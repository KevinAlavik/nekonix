#ifndef PRINTF_H
#define PRINTF_H

#include <lib/types.h>
#include <stdarg.h>

// Kernel printf wrappers to nanoprintf (include/lib/_internal/nanoprintf.h)

extern int (*putchar_impl)(char c);

int putchar(char c);
int puts(const char *str);
int printf(const char *format, ...);
int vprintf(const char *format, va_list args);

#endif // PRINTF_H