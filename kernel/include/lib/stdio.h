#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <lib/types.h>

extern void *stdout;
extern u16 _stdout_port;
extern spinlock_t stdout_lock;

extern int (*putchar_impl)(char c);

int puts(const char *s);
int vprintf(const char *fmt, va_list args);
int printf(const char *fmt, ...);

// DEPRECATED: use normal printf instead.
int f_printf(const char *fmt, ...);

int vfprintf(void *stream, const char *fmt, va_list args);
int fprintf(void *stream, const char *fmt, ...);
int snprintf(char *str, usize size, const char *fmt, ...);

#endif // STDIO_H
