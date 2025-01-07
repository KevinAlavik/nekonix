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

#ifndef IDT_H
#define IDT_H

#include <lib/types.h>

#define IDT_ENTRY_COUNT 256
#define IRQ_BASE 0x20
#define IRQ_COUNT 16

typedef struct
{
    u16 offsetLow;
    u16 selector;
    u8 ist;
    u8 flags;
    u16 offsetMiddle;
    u32 offsetHigh;
    u32 zero;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    u16 limit;
    u64 base;
} __attribute__((packed)) idt_pointer_t;

typedef struct
{
    u64 ds;
    u64 cr2;
    u64 cr3;

    u64 r15;
    u64 r14;
    u64 r13;
    u64 r12;
    u64 r11;
    u64 r10;
    u64 r9;
    u64 r8;
    u64 rbp;
    u64 rdi;
    u64 rsi;
    u64 rdx;
    u64 rcx;
    u64 rbx;
    u64 rax;

    u64 vector;
    u64 err;

    u64 rip;
    u64 cs;
    u64 rflags;
    u64 rsp;
    u64 ss;
} __attribute__((packed)) int_frame_t;

extern idt_entry_t idt_entries[IDT_ENTRY_COUNT];

typedef void (*idt_handler_t)(int_frame_t *);
int idt_init();
void idt_load(u64);

void idt_set_gate(idt_entry_t idt[], int num, u64 base, u16 sel, u8 flags);

void idt_irq_register(int irq, idt_handler_t handler);
void idt_irq_deregister(int irq);

#endif // IDT_H