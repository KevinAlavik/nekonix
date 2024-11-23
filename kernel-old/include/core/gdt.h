#ifndef GDT_H
#define GDT_H

#include <lib/types.h>

typedef struct
{
    u16 limit;
    uptr base;
} __attribute((packed)) gdtpr_t;

typedef struct
{
    u16 limitLow;
    u16 baseLow;
    u8 baseMid;
    u8 accessByte;
    u8 limitHighAndFlags;
    u8 baseHigh;
} __attribute((packed)) gdt_entry_t;

extern gdtpr_t gdtr;

int gdt_init();
void gdt_flush(gdtpr_t gdtr);

#endif // GDT_H