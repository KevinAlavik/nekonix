#include <lib/stdio.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <dev/serial.h>
#include <proc/scheduler.h>

#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182
#define IRQ0 0

extern int serial_putchar(char);
extern int flanterm_putchar(char);
static void timer_interrupt_handler(int_frame_t *frame)
{
    scheduler_tick(frame);
    // pic_send_end_of_interrupt(IRQ0);
}

void timer_init(int frequency)
{
    u16 divisor = PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, 0x36);                    // Set PIT to Mode 3 (Square Wave Generator)
    outb(PIT_CHANNEL_0, divisor & 0xFF);        // Send low byte of divisor
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF); // Send high byte of divisor

    pic_configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, true);
    pic_unmask(IRQ0);

    idt_irq_register(IRQ0, timer_interrupt_handler);
}

void test_proc()
{
    printf("A");
}

void test()
{
    putchar_impl = serial_putchar;
    scheduler_init();
    timer_init(100);
    proc_spawn(test_proc);
    __asm__ volatile("int $0x20"); // Kickstart the scheduler
}
