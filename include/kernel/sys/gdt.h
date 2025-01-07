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

#ifndef GDT_H
#define GDT_H

#include <lib/types.h>

typedef struct gdt_entry
{
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_ptr
{
    u16 limit;
    uptr base;
} __attribute__((packed)) gdt_ptr_t;

extern gdt_ptr_t gdtpr;

int gdt_init();
void gdt_flush(gdt_ptr_t *gdtpr);

#endif // GDT_H