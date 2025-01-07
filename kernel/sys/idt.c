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

#include <sys/idt.h>
#include <sys/idt.h>
#include <sys/cpu.h>
#define KLOG_MODULE "interrupts"
#include <utils/log.h>
#include <utils/panic.h>

idt_entry_t idt_entries[IDT_ENTRY_COUNT];
idt_pointer_t idt_pointer;
extern u64 isr_table[];

idt_handler_t irq_handlers[IRQ_COUNT] = {0};

typedef struct
{
    const char *message;
    const char *mnemonic;
} exception_info_t;

static const exception_info_t exception_info[32] = {
    {"Division By Zero", "#DE"},
    {"Debug", "#DB"},
    {"Non-maskable interrupt", "-"},
    {"Breakpoint", "#BP"},
    {"Overflow", "#OF"},
    {"Bound Range Exceeded", "#BR"},
    {"Invalid Opcode", "#UD"},
    {"Device Not Available", "#NM"},
    {"Double Fault", "#DF"},
    {"Coprocessor Segment Overrun", "-"},
    {"Invalid TSS", "#TS"},
    {"Segment Not Present", "#NP"},
    {"Stack Segment Fault", "#SS"},
    {"General Protection Fault", "#GP"},
    {"Page Fault", "#PF"},
    {"Reserved", "-"},
    {"x87 FPU Error", "#MF"},
    {"Alignment Check", "#AC"},
    {"Machine Check", "#MC"},
    {"SIMD Floating-Point Exception", "#XM/#XF"},
    {"Virtualization Exception", "#VE"},
    {"Control Protection Exception", "#CP"},
    {"Reserved", "-"},
    {"Reserved", "-"},
    {"Reserved", "-"},
    {"Reserved", "-"},
    {"Reserved", "-"},
    {"Reserved", "-"},
    {"Hypervisor Injection Exception", "#HV"},
    {"VMM Communication Exception", "#VC"},
    {"Security Exception", "#SX"},
    {"Reserved", "-"}};

void idt_set_gate(idt_entry_t idt[], int num, u64 base, u16 sel, u8 flags)
{
    idt[num].offsetLow = (base & 0xFFFF);
    idt[num].offsetMiddle = (base >> 16) & 0xFFFF;
    idt[num].offsetHigh = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].zero = 0;
}

int idt_init()
{
    idt_pointer.limit = sizeof(idt_entry_t) * IDT_ENTRY_COUNT - 1;
    idt_pointer.base = (uptr)&idt_entries;

    for (size_t i = 0; i < IRQ_COUNT; i++)
    {
        irq_handlers[i] = NULL;
    }

    for (int i = 0; i < 32; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
    }

    for (int i = IRQ_BASE; i < IRQ_BASE + IRQ_COUNT; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
    }

    idt_set_gate(idt_entries, 0x80, isr_table[0x80], 0x08, 0xEE);

    idt_load((u64)&idt_pointer);
    DEBUG("IDT loaded with base=0x%lx, limit=%u", idt_pointer.base, idt_pointer.limit);
    return 0;
}

void idt_handler(int_frame_t frame)
{
    if (frame.vector < 32)
    {
        panic(&frame, exception_info[frame.vector].mnemonic, exception_info[frame.vector].message, true);
    }
    else if (frame.vector >= IRQ_BASE && frame.vector < IRQ_BASE + IRQ_COUNT)
    {
        WARN("Received IRQ%d, dropping.", frame.vector - IRQ_BASE);
    }
    else if (frame.vector == 0x80)
    {
        WARN("Recived system call interrupt, no action taken");
    }
    else
    {
        WARN("Received unknown interrupt vector: %d, no action taken", frame.vector);
    }
}

void idt_irq_register(int irq, idt_handler_t handler)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("Attempted to register invalid IRQ: %d", irq);
        return;
    }
    irq_handlers[irq] = handler;
    DEBUG("Registered IRQ handler for IRQ %d", irq);
}

void idt_irq_deregister(int irq)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("Attempted to deregister invalid IRQ: %d", irq);
        return;
    }
    irq_handlers[irq] = 0;
    DEBUG("Deregistered IRQ handler for IRQ %d", irq);
}