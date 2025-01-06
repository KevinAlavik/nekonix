#ifndef CPU_H
#define CPU_H

static inline void halt()
{
    __asm__ volatile("hlt");
}

static inline void cli()
{
    __asm__ volatile("cli");
}

static inline void sti()
{
    __asm__ volatile("sti");
}

static inline void hcf()
{
    cli();
    for (;;)
    {
        halt();
    }
}

#endif // CPU_H