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

#include <sys/gdt.h>
#define KLOG_MODULE "gdt"
#include <utils/log.h>

gdt_ptr_t gdtpr;
gdt_entry_t gdtEntries[5];

int gdt_init()
{
    gdtEntries[0] = (gdt_entry_t){0, 0, 0, 0x00, 0x00, 0};
    gdtEntries[1] = (gdt_entry_t){0, 0, 0, 0x9A, 0xA0, 0};
    gdtEntries[2] = (gdt_entry_t){0, 0, 0, 0x92, 0xA0, 0};
    gdtEntries[3] = (gdt_entry_t){0, 0, 0, 0xFA, 0xA0, 0};
    gdtEntries[4] = (gdt_entry_t){0, 0, 0, 0xF2, 0xA0, 0};

    gdtpr.limit = (u16)(sizeof(gdtEntries) - 1);
    gdtpr.base = (uptr)&gdtEntries;

    gdt_flush(&gdtpr);

    DEBUG("GDT loaded with base=0x%lx, limit=%u", gdtpr.base, gdtpr.limit);
    return 0;
}

void gdt_flush(gdt_ptr_t *gdtpr)
{
    __asm__ volatile("mov %0, %%rdi\n"
                     "lgdt (%%rdi)\n"
                     "push $0x8\n"
                     "lea 1f(%%rip), %%rax\n"
                     "push %%rax\n"
                     "lretq\n"
                     "1:\n"
                     "mov $0x10, %%ax\n"
                     "mov %%ax, %%es\n"
                     "mov %%ax, %%ss\n"
                     "mov %%ax, %%gs\n"
                     "mov %%ax, %%ds\n"
                     "mov %%ax, %%fs\n"
                     :
                     : "r"(gdtpr)
                     : "memory");
}