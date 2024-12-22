#include <sys/cpu.h>

void hlt()
{
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

void hcf()
{
    __asm__ volatile("cli");
    hlt();
}