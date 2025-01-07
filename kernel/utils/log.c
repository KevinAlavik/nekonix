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

#include <utils/log.h>
#include <stdarg.h>
#include <utils/printf.h>
#include <nnix.h>

void _log_callback(int log_level, bool debug, const char *file, int line, const char *level, const char *scope, const char *color, const char *format, ...)
{
    if (kernel_log_level >= log_level && (!debug || (debug && kernel_debug_enabled)))
    {
        va_list args;
        va_start(args, format);

        // (file:line) LEVEL @ scope: MESSAGE, with color

        if (debug || kernel_debug_enabled)
        {
            printf("%s[%s:%d] %s @ %s%s: ", color, file, line, level, scope, ANSI_COLOR_RESET);
        }
        else
        {
            printf("%s%s @ %s%s: ", color, level, scope, ANSI_COLOR_RESET);
        }

        vprintf(format, args);
        printf("\n");

        va_end(args);
    }
}
