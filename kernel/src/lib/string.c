#include <lib/string.h>
#include <boot/nnix.h>

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

usize strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return (usize)(p - s);
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

char *strncpy(char *dest, const char *src, usize n)
{
    char *d = dest;
    while (n && (*d++ = *src++))
        n--;
    while (n--)
        *d++ = '\0';
    return dest;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, usize n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        n--;
    }
    return (n == 0) ? 0 : (*(unsigned char *)s1 - *(unsigned char *)s2);
}
