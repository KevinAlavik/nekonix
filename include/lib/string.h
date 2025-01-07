/*
 * Copyright (c) 2024 Kevin Alavik <kevin@alavik.se>
 *
 * Licensed under the Nekonix License-v1
 * See the LICENSE file for more details.
 *
 * You are allowed to use, modify, and distribute this software in both private and commercial environments,
 * as long as you retain the copyright notice and do not remove or alter any copyright notice or attribution.
 * This software is provided "as-is" without warranty of any kind.
 */

#ifndef STRING_H
#define STRING_H

#include <lib/types.h>

void *memcpy(void *dest, const void *src, usize n);
void *memset(void *s, int c, usize n);
int memcmp(const void *s1, const void *s2, usize n);
char *strcpy(char *dest, const char *src);
usize strlen(const char *str);
int strcmp(const char *s1, const char *s2);
char *strtok(char *str, const char *delim);
char *strcat(char *dest, const char *src);
char *strchr(const char *str, int c);
char *strrchr(const char *str, int c);
usize strspn(const char *str, const char *delim);
usize strcspn(const char *str, const char *delim);
int strncasecmp(const char *s1, const char *s2, usize n);
int strcasecmp(const char *s1, const char *s2);

#endif // STRING_H