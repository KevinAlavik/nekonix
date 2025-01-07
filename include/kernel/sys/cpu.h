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

#ifndef CPU_H
#define CPU_H

static inline void halt()
{
    __asm__ volatile("hlt");
}

static inline void cli()
{
    __asm__ volatile("cli");
}

static inline void sti()
{
    __asm__ volatile("sti");
}

static inline void hcf()
{
    cli();
    for (;;)
    {
        halt();
    }
}

#endif // CPU_H