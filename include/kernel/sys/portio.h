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

#ifndef PORTIO_H
#define PORTIO_H

#include <lib/types.h>

void outb(u16 port, u8 data);
u8 inb(u16 port);
void outw(u16 port, u16 data);
u16 inw(u16 port);
void outl(u16 port, u32 data);
u32 inl(u16 port);

#endif // PORTIO_H
