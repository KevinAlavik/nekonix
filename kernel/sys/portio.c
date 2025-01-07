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

#include <sys/portio.h>

// Generic port I/O using the x86_64 instructions.
void outb(u16 port, u8 data)
{
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

u8 inb(u16 port)
{
    u8 data;
    __asm__ volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void outw(u16 port, u16 data)
{
    __asm__ volatile("outw %0, %1" : : "a"(data), "Nd"(port));
}

u16 inw(u16 port)
{
    u16 data;
    __asm__ volatile("inw %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void outl(u16 port, u32 data)
{
    __asm__ volatile("outl %0, %1" : : "a"(data), "Nd"(port));
}

u32 inl(u16 port)
{
    u32 data;
    __asm__ volatile("inl %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}
