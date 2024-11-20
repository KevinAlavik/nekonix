#include <dev/serial.h>

// Write an 8-bit value to the specified port
void outb(u16 port, u8 value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Write a 16-bit value to the specified port
void outw(u16 port, u16 value)
{
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}

// Write a 32-bit value to the specified port
void outl(u16 port, u32 value)
{
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

// Read an 8-bit value from the specified port
u8 inb(u16 port)
{
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Read a 16-bit value from the specified port
u16 inw(u16 port)
{
    u16 ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Read a 32-bit value from the specified port
u32 inl(u16 port)
{
    u32 ret;
    asm volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void io_wait()
{
    outb(0x80, 0);
}