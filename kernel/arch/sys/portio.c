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
