#ifndef SERIAL_H
#define SERIAL_H

#include <lib/types.h>

// Output functions
void outb(u16 port, u8 value);
void outw(u16 port, u16 value);
void outl(u16 port, u32 value);

// Input functions
u8 inb(u16 port);
u16 inw(u16 port);
u32 inl(u16 port);

void io_wait();

#endif // SERIAL_H
