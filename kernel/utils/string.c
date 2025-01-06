#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;

    while (n--)
    {
        *d++ = *s++;
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;

    while (n--)
    {
        *p++ = (unsigned char)c;
    }

    return s;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;

    while (n--)
    {
        if (*p1 != *p2)
        {
            return (*p1 < *p2) ? -1 : 1;
        }
        p1++;
        p2++;
    }

    return 0;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++) != '\0')
        ;
    return dest;
}

size_t strlen(const char *str)
{
    const char *s = str;

    while (*s)
    {
        s++;
    }

    return s - str;
}
