#include <dev/serial.h>

void outb(u16 port, u8 value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}