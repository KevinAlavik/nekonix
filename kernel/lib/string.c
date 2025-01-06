#include <lib/string.h>
#include <lib/ctype.h>

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

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

char *strtok(char *str, const char *delim)
{
    static char *last;
    if (str)
    {
        last = str;
    }
    else
    {
        if (!last)
        {
            return NULL;
        }
        str = last;
    }

    str += strspn(str, delim);
    last = str + strcspn(str, delim);
    if (*last)
    {
        *last++ = '\0';
    }
    else
    {
        last = NULL;
    }

    return str;
}

char *strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d)
    {
        d++;
    }
    while ((*d++ = *src++) != '\0')
        ;
    return dest;
}

char *strchr(const char *str, int c)
{
    while (*str)
    {
        if (*str == (char)c)
        {
            return (char *)str;
        }
        str++;
    }
    return NULL;
}

char *strrchr(const char *str, int c)
{
    const char *last = NULL;
    while (*str)
    {
        if (*str == (char)c)
        {
            last = str;
        }
        str++;
    }
    return (char *)last;
}

size_t strspn(const char *str, const char *delim)
{
    const char *s = str;
    while (*s && strchr(delim, *s))
    {
        s++;
    }
    return s - str;
}

size_t strcspn(const char *str, const char *delim)
{
    const char *s = str;
    while (*s && !strchr(delim, *s))
    {
        s++;
    }
    return s - str;
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n--)
    {
        char c1 = tolower(*s1);
        char c2 = tolower(*s2);

        if (c1 != c2)
        {
            return c1 - c2;
        }

        if (*s1 == '\0')
        {
            break;
        }

        s1++;
        s2++;
    }

    return 0;
}

int strcasecmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2 || tolower(*s1) == tolower(*s2)))
    {
        s1++;
        s2++;
    }

    return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
