#include <utils/printf.h>
#include <lib/types.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <lib/_internal/nanoprintf.h>

int (*putchar_impl)(char c) = NULL;
spinlock_t stdout_lock;

int putchar(char c)
{
    if (putchar_impl)
    {
        return putchar_impl(c);
    }
    return -1;
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
    char buffer[1024];
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (length >= 0 && length < (int)sizeof(buffer))
    {
        for (int i = 0; i < length; i++)
        {
            putchar(buffer[i]);
        }
    }

    return length;
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