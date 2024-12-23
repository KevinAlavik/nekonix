#include <lib/stdio.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <dev/serial.h>
#include <sys/cpu.h>
#include <proc/scheduler.h>

#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182
#define IRQ0 0

extern int serial_putchar(char);
extern int flanterm_putchar(char);

static volatile u64 tick_count = 0;

static void timer_interrupt_handler(int_frame_t *frame)
{
    tick_count++;
    scheduler_context_switch(frame);
    pic_send_end_of_interrupt(IRQ0); // Send End of Interrupt signal to PIC
}

void timer_init(int frequency)
{
    if (frequency < 18 || frequency > 1000)
    {
        frequency = 100; // Default to 100Hz if out of range
    }

    u16 divisor = PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, 0x36);                    // Set PIT to Mode 3 (Square Wave Generator)
    outb(PIT_CHANNEL_0, divisor & 0xFF);        // Send low byte of divisor
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF); // Send high byte of divisor

    pic_configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, true);
    pic_unmask(IRQ0);

    idt_irq_register(IRQ0, timer_interrupt_handler);
}

u64 timer_get_ticks()
{
    return tick_count;
}

void timer_sleep(u64 ticks)
{
    u64 start = timer_get_ticks();
    while (timer_get_ticks() < start + ticks)
    {
        __asm__ volatile("hlt");
    }
}

void procA()
{
    while (1)
    {
        flanterm_putchar('A');
        timer_sleep(5);
    }
}

void procB()
{
    while (1)
    {
        serial_putchar('B');
        timer_sleep(5);
    }
}

void test()
{

    scheduler_init();
    timer_init(100);
    scheduler_create_process(procA);
    scheduler_create_process(procB);

    __asm__ volatile("int $0x20");
}
