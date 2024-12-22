#include <sys/gdt.h>
#include <boot/nnix.h>

gdtpr_t gdtpr;
gdt_entry_t gdtEntries[5];

int gdt_init()
{
    gdtEntries[0] = (gdt_entry_t){0, 0, 0, 0, 0, 0};
    gdtEntries[1] = (gdt_entry_t){0, 0, 0, 0b10011010, 0b10100000, 0};
    gdtEntries[2] = (gdt_entry_t){0, 0, 0, 0b10010010, 0b10100000, 0};
    gdtEntries[3] = (gdt_entry_t){0, 0, 0, 0b11111010, 0b10100000, 0};
    gdtEntries[4] = (gdt_entry_t){0, 0, 0, 0b11110010, 0b10100000, 0};

    gdtpr.limit = (u16)(sizeof(gdtEntries) - 1);
    gdtpr.base = (uptr)&gdtEntries;

    gdt_flush(gdtpr);

    DEBUG("interrupt", "GDT loaded with base=0x%lx, limit=%u", gdtpr.base, gdtpr.limit);
    return 0;
}

void gdt_flush(gdtpr_t gdtr)
{
    __asm__ volatile("mov %0, %%rdi\n"
                     "lgdt (%%rdi)\n"
                     "push $0x8\n"
                     "lea 1f(%%rip), %%rax\n"
                     "push %%rax\n"
                     "lretq\n"
                     "1:\n"
                     "mov $0x10, %%ax\n"
                     "mov %%ax, %%es\n"
                     "mov %%ax, %%ss\n"
                     "mov %%ax, %%gs\n"
                     "mov %%ax, %%ds\n"
                     "mov %%ax, %%fs\n"
                     :
                     : "r"(&gdtr)
                     : "memory");
}