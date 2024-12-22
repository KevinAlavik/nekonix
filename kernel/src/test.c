#include <lib/stdio.h>
#include <sys/idt.h>
#include <dev/serial.h>
#include <sys/cpu.h>
#include <proc/scheduler.h>

void proc_test()
{
    printf("B");
}

void test()
{
    scheduler_initialize();
    proc_spawn(proc_test);
    proc_spawn(proc_test);
    __asm__ volatile("int $0x01");
}
