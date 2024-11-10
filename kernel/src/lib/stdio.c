#include <lib/stdio.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <lib/nanoprintf.h>

#include <dev/serial.h>
#include <dev/serial_util.h>

// TODO: Implement proper FILE streams.

void *stdout = NULL;
spinlock_t stdout_lock;

int putchar(int c)
{
    outb(0x3F8, c);
    return c;
}

int puts(const char *str)
{
    spinlock_acquire(&stdout_lock);
    int length = 0;
    for (length = 0; *str; length++, putchar(*str++))
        ;
    putchar('\n');
    spinlock_release(&stdout_lock);
    return length + 1;
}

int vprintf(const char *fmt, va_list args)
{
    return vfprintf(stdout, fmt, args);
}

int printf(const char *fmt, ...)
{
    spinlock_acquire(&stdout_lock);
    va_list args;
    va_start(args, fmt);
    int length = vprintf(fmt, args);
    va_end(args);
    spinlock_release(&stdout_lock);
    return length;
}

int vfprintf(void *stream, const char *fmt, va_list args)
{
    (void)stream;

    char buffer[1024];
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (length >= 0 && length < (int)sizeof(buffer))
    {
        outstr(0x3F8, buffer);
    }

    return length;
}

int fprintf(void *stream, const char *fmt, ...)
{
    spinlock_acquire(&stdout_lock);
    va_list args;
    va_start(args, fmt);
    int length = vfprintf(stream, fmt, args);
    va_end(args);
    spinlock_release(&stdout_lock);
    return length;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
    spinlock_acquire(&stdout_lock);
    va_list args;
    va_start(args, fmt);
    int length = npf_vsnprintf(str, size, fmt, args);
    va_end(args);
    spinlock_release(&stdout_lock);
    return length;
}
