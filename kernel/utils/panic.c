#include <utils/panic.h>
#define KLOG_MODULE "knrlpanic"
#include <utils/log.h>
#include <utils/printf.h>
#include <nnix.h>
#include <sys/cpu.h>

char *panic_logo[] = {
    " _                        _                     _      ",
    "| | _____ _ __ _ __   ___| |  _ __   __ _ _ __ (_) ___ ",
    "| |/ / _ \\ '__| '_ \\ / _ \\ | | '_ \\ / _` | '_ \\| |/ __|",
    "|   <  __/ |  | | | |  __/ | | |_) | (_| | | | | | (__ ",
    "|_|\\_\\___|_|  |_| |_|\\___|_| | .__/ \\__,_|_| |_|_|\\___|",
    "                             |_|                       ",
};

void dump_registers(int_frame_t *frame)
{
    if (frame)
    {
        PANIC("+--------------------+--------------------+--------------------+--------------------+");
        PANIC("|      Register      |       Value        |      Register      |       Value        |");
        PANIC("|--------------------|--------------------|--------------------|--------------------|");
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "ds", frame->ds, "cr2", frame->cr2);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "cr3", frame->cr3, "r15", frame->r15);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "r14", frame->r14, "r13", frame->r13);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "r12", frame->r12, "r11", frame->r11);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "r10", frame->r10, "r9", frame->r9);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "r8", frame->r8, "rbp", frame->rbp);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "rdi", frame->rdi, "rsi", frame->rsi);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "rdx", frame->rdx, "rcx", frame->rcx);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "rbx", frame->rbx, "rax", frame->rax);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "vector", frame->vector, "err", frame->err);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "rip", frame->rip, "cs", frame->cs);
        PANIC("| %-18s | 0x%016lx | %-18s | 0x%016lx |", "rflags", frame->rflags, "rsp", frame->rsp);
        PANIC("| %-18s | 0x%016lx | %-18s | %-18s |", "ss", frame->ss, "-", "-");
        PANIC("+--------------------+--------------------+--------------------+--------------------+");
    }
}

void panic(int_frame_t *frame, const char *kind, const char *message, bool halt)
{
    printf("%s", ANSI_COLOR_RED);
    for (size_t i = 0; i < sizeof(panic_logo) / sizeof(panic_logo[0]); ++i)
    {
        printf("%s\n", panic_logo[i]);
    }
    printf("%s\n", ANSI_COLOR_RESET);

    if (frame)
        PANIC("%s: %s @ 0x%.16llx on CPU %s", kind, message, frame->rip, "<unknown>");
    else
        PANIC("%s: %s @ <???> on CPU %s", kind, message, "<unknown>");

    if (frame)
    {
        dump_registers(frame);
    }

    if (halt)
        hcf();
}
