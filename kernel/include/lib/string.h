#ifndef STRING_H
#define STRING_H

#include <lib/types.h>

void *memcpy(void *dest, const void *src, usize n);
void *memset(void *s, int c, usize n);
void *memmove(void *dest, const void *src, usize n);
int memcmp(const void *s1, const void *s2, usize n);

#endif // STRING_H