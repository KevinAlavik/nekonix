#ifndef PORTIO_H
#define PORTIO_H

#include <lib/types.h>

void outb(u16 port, u8 data);
u8 inb(u16 port);
void outw(u16 port, u16 data);
u16 inw(u16 port);
void outl(u16 port, u32 data);
u32 inl(u16 port);

#endif // PORTIO_H
