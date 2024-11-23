#ifndef STRING_H
#define STRING_H

#include <lib/types.h>
#include <lib/stdlib.h>

void *memcpy(void *dest, const void *src, usize n);
void *memset(void *s, int c, usize n);
void *memmove(void *dest, const void *src, usize n);
int memcmp(const void *s1, const void *s2, usize n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, usize n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, usize n);
usize strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, usize n);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
usize strcspn(const char *s, const char *reject);
usize strspn(const char *s, const char *accept);
char *strpbrk(const char *s, const char *accept);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *str, const char *delim);
char *strdup(const char *s);
char *strndup(const char *s, usize n);
long strtol(const char *str, char **endptr, int base);

#endif // STRING_H
