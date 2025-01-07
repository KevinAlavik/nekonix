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

#ifndef NNIX_H
#define NNIX_H

#include <lib/string.h>
#include <utils/printf.h>
#include <utils/flanterm/flanterm.h>

#define LIMINE_API_REVISION 2

struct cmdline_arg
{
    char *name;
    char *value;
};

static inline void parse_cmdline(char *cmdline, struct cmdline_arg argv[])
{
    char *token;
    int i = 0;
    bool inside_quotes = false;
    char current_value[256] = {0};

    token = strtok(cmdline, " ");
    while (token != NULL)
    {
        if (inside_quotes)
        {
            if (strchr(token, '"') != NULL)
            {
                char *end_quote = strchr(token, '"');
                *end_quote = '\0';
                strcat(current_value, token);
                argv[i].value = current_value;
                inside_quotes = false;
                i++;
            }
            else
            {
                strcat(current_value, token);
                strcat(current_value, " ");
            }
        }
        else
        {
            char *equal_sign = strchr(token, '=');
            if (equal_sign != NULL)
            {
                *equal_sign = '\0';
                argv[i].name = token;
                argv[i].value = equal_sign + 1;

                if (*argv[i].value == '"')
                {
                    argv[i].value++;
                    inside_quotes = true;
                    memset(current_value, 0, sizeof(current_value));
                    strcpy(current_value, argv[i].value);
                }
                else
                {
                    i++;
                }
            }
            else
            {
                argv[i].name = token;
                argv[i].value = NULL;
                i++;
            }
        }

        token = strtok(NULL, " ");
    }
    argv[i].name = NULL;
}

extern struct cmdline_arg kernel_args[10];
extern int kernel_arg_count;

static inline int atoi(const char *str)
{
    int result = 0;
    int sign = 1;

    if (str == NULL || *str == '\0')
        return 0;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }

    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

static inline bool cmdline_has_arg(const char *name)
{
    for (size_t i = 0; kernel_args[i].name != NULL; i++)
    {
        if (strcmp(kernel_args[i].name, name) == 0)
        {
            return true;
        }
    }
    return false;
}

static inline bool cmdline_lacks_arg(const char *name)
{
    return !cmdline_has_arg(name);
}

static inline const char *cmdline_get_string(const char *name, const char *default_value)
{
    for (size_t i = 0; kernel_args[i].name != NULL; i++)
    {
        if (strcmp(kernel_args[i].name, name) == 0)
        {
            return kernel_args[i].value;
        }
    }
    return default_value;
}

static inline int cmdline_get_int(const char *name, int default_value)
{
    for (size_t i = 0; kernel_args[i].name != NULL; i++)
    {
        if (strcmp(kernel_args[i].name, name) == 0)
        {
            return atoi(kernel_args[i].value);
        }
    }
    return default_value;
}

static inline bool cmdline_get_bool(const char *name, bool default_value)
{
    for (size_t i = 0; kernel_args[i].name != NULL; i++)
    {
        if (strcmp(kernel_args[i].name, name) == 0)
        {
            return (strcmp(kernel_args[i].value, "true") == 0);
        }
    }
    return default_value;
}

extern bool kernel_debug_enabled;
extern int kernel_log_level;

#define VERSION_MAJOR "1"
#define VERSION_MINOR "0"
#define VERSION_PATCH "0"
#define VERSION_EXTRA "-unstable-alpha"

extern struct flanterm_context *ft_ctx;
extern u64 hhdm_offset;

#define DEBUG_MARK(mark, fmt, ...) (s_printf("@DEBUG@%s ", mark), s_printf(fmt, ##__VA_ARGS__), s_printf("\n"))

#endif
