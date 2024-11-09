#include <lib/string.h>

void *memcpy(void *dest, const void *src, usize n)
{
    u8 *d = dest;
    const u8 *s = src;
    for (; n--;)
    {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *s, int c, usize n)
{
    u8 *p = s;
    while (n--)
    {
        *p++ = (u8)c;
    }
    return s;
}

void *memmove(void *dest, const void *src, usize n)
{
    u8 *d = dest;
    const u8 *s = src;
    if (s > d)
    {
        while (n--)
            *d++ = *s++;
    }
    else if (s < d)
    {
        d += n;
        s += n;
        while (n--)
            *--d = *--s;
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, usize n)
{
    const u8 *p1 = s1;
    const u8 *p2 = s2;
    while (n--)
    {
        if (*p1 != *p2)
            return (*p1 < *p2) ? -1 : 1;
        p1++, p2++;
    }
    return 0;
}
