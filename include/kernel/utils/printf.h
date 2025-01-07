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

#ifndef PRINTF_H
#define PRINTF_H

#include <lib/types.h>
#include <stdarg.h>

// Kernel printf wrappers to nanoprintf (include/lib/_internal/nanoprintf.h)

extern int (*putchar_impl)(char c);

int putchar(char c);
int puts(const char *str);
int printf(const char *format, ...);
int vprintf(const char *format, va_list args);

#endif // PRINTF_H