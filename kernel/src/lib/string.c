#include <lib/string.h>
#include <boot/nnix.h>
#include <limits.h>

void *memcpy(void *dest, const void *src, usize n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}

void *memset(void *s, int c, usize n)
{
    unsigned char *p = s;
    while (n--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memmove(void *dest, const void *src, usize n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s)
    {
        while (n--)
        {
            *d++ = *s++;
        }
    }
    else
    {
        d += n;
        s += n;
        while (n--)
        {
            *(--d) = *(--s);
        }
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, usize n)
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

// String manipulation functions
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
    {
        n--;
    }
    while (n--)
    {
        *d++ = '\0';
    }
    return dest;
}

char *strcat(char *dest, const char *src)
{
    char *d = dest;
    while (*d)
    {
        d++;
    }
    while ((*d++ = *src++))
        ;
    return dest;
}

char *strncat(char *dest, const char *src, usize n)
{
    char *d = dest;
    while (*d)
    {
        d++;
    }
    while (n-- && (*d++ = *src++))
        ;
    *d = '\0';
    return dest;
}

usize strlen(const char *s)
{
    const char *p = s;
    while (*p)
    {
        p++;
    }
    return p - s;
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
    return n ? (*(unsigned char *)s1 - *(unsigned char *)s2) : 0;
}

char *strchr(const char *s, int c)
{
    while (*s)
    {
        if (*s == (char)c)
        {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

char *strrchr(const char *s, int c)
{
    const char *last = NULL;
    while (*s)
    {
        if (*s == (char)c)
        {
            last = s;
        }
        s++;
    }
    return (char *)last;
}

usize strcspn(const char *s, const char *reject)
{
    const char *p;
    usize count = 0;

    while (*s)
    {
        for (p = reject; *p; p++)
        {
            if (*s == *p)
            {
                return count;
            }
        }
        s++;
        count++;
    }
    return count;
}

usize strspn(const char *s, const char *accept)
{
    const char *p;
    usize count = 0;

    while (*s)
    {
        for (p = accept; *p; p++)
        {
            if (*s == *p)
            {
                count++;
                break;
            }
        }
        if (!*p)
        {
            break;
        }
        s++;
    }
    return count;
}

char *strpbrk(const char *s, const char *accept)
{
    while (*s)
    {
        const char *a = accept;
        while (*a)
        {
            if (*s == *a)
            {
                return (char *)s;
            }
            a++;
        }
        s++;
    }
    return NULL;
}

char *strstr(const char *haystack, const char *needle)
{
    if (!*needle)
    {
        return (char *)haystack;
    }
    for (; *haystack; haystack++)
    {
        const char *h = haystack;
        const char *n = needle;

        while (*h && *n && (*h == *n))
        {
            h++;
            n++;
        }
        if (!*n)
        {
            return (char *)haystack;
        }
    }
    return NULL;
}

char *strtok(char *str, const char *delim)
{
    static char *last;
    char *end;

    if (str == NULL)
    {
        str = last;
    }
    if (str == NULL)
    {
        return NULL;
    }

    str += strspn(str, delim);
    if (*str == '\0')
    {
        return NULL;
    }

    end = str + strcspn(str, delim);
    if (*end)
    {
        *end++ = '\0';
    }

    last = end;
    return str;
}

char *strdup(const char *s)
{
    usize len = strlen(s);
    char *copy = (char *)kmalloc(len + 1);
    if (copy)
    {
        strcpy(copy, s);
    }
    return copy;
}

char *strndup(const char *s, usize n)
{
    usize len = strlen(s);
    if (len > n)
    {
        len = n;
    }
    char *copy = (char *)kmalloc(len + 1);
    if (copy)
    {
        strncpy(copy, s, len);
        copy[len] = '\0';
    }
    return copy;
}

long strtol(const char *str, char **endptr, int base)
{
    const char *s = str;
    long result = 0;
    int sign = 1;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r' || *s == '\f' || *s == '\v')
    {
        s++;
    }

    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    else if (*s == '+')
    {
        s++;
    }

    if (base == 0)
    {
        if (*s == '0')
        {
            if (*(s + 1) == 'x' || *(s + 1) == 'X')
            {
                base = 16;
                s += 2;
            }
            else
            {
                base = 8;
                s++;
            }
        }
        else
        {
            base = 10;
        }
    }
    else if (base == 16 && *s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X'))
    {
        s += 2;
    }

    long cutoff = LONG_MAX / base;
    int cutlim = LONG_MAX % base;

    while (*s)
    {
        int digit;

        if (*s >= '0' && *s <= '9')
        {
            digit = *s - '0';
        }
        else if (*s >= 'a' && *s <= 'z')
        {
            digit = *s - 'a' + 10;
        }
        else if (*s >= 'A' && *s <= 'Z')
        {
            digit = *s - 'A' + 10;
        }
        else
        {
            break;
        }

        if (digit >= base)
        {
            break;
        }

        if (result > cutoff || (result == cutoff && digit > cutlim))
        {
            result = sign == 1 ? LONG_MAX : LONG_MIN;
            if (endptr)
            {
                *endptr = (char *)s;
            }
            return result;
        }

        result = result * base + digit;
        s++;
    }

    if (endptr)
    {
        *endptr = (char *)s;
    }

    return result * sign;
}
