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
u16 _stdout_port = 0x3F8;
spinlock_t stdout_lock;

int (*putchar_impl)(char c) = NULL;

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

// DEPRECATED: use normal printf instead.
extern int flanterm_putchar(char);
int f_printf(const char *fmt, ...)
{

    spinlock_acquire(&stdout_lock);
    va_list args;
    va_start(args, fmt);

    char buffer[1024];
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (length >= 0 && length < (int)sizeof(buffer))
    {
        for (int i = 0; i < length; i++)
        {
            flanterm_putchar(buffer[i]);
        }
    }

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
        for (int i = 0; i < length; i++)
        {
            putchar(buffer[i]);
        }
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

int snprintf(char *str, usize size, const char *fmt, ...)
{
    spinlock_acquire(&stdout_lock);
    va_list args;
    va_start(args, fmt);
    int length = npf_vsnprintf(str, size, fmt, args);
    va_end(args);
    spinlock_release(&stdout_lock);
    return length;
}
