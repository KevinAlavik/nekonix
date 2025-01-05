#include <sys/idt.h>
#include <boot/nnix.h>
#include <sys/cpu.h>
#include <mm/pmm.h>
#include <proc/scheduler.h>
#include <sys/pic.h>
#include <sys/syscall.h>

idt_entry_t idt_entries[IDT_ENTRY_COUNT];
idt_pointer_t idt_pointer;
extern u64 isr_table[];

bool _panicked;

idt_handler_t irq_handlers[IRQ_COUNT] = {0};

typedef struct
{
    const char *message;
    const char *mnemonic;
    bool fatal;
} exception_info_t;

static const exception_info_t exception_info[32] = {
    {"Division By Zero", "#DE", true},
    {"Debug", "#DB", false},
    {"Non-maskable interrupt", "-", true},
    {"Breakpoint", "#BP", false},
    {"Overflow", "#OF", true},
    {"Bound Range Exceeded", "#BR", true},
    {"Invalid Opcode", "#UD", true},
    {"Device Not Available", "#NM", true},
    {"Double Fault", "#DF", true},
    {"Coprocessor Segment Overrun", "-", true},
    {"Invalid TSS", "#TS", true},
    {"Segment Not Present", "#NP", true},
    {"Stack Segment Fault", "#SS", true},
    {"General Protection Fault", "#GP", true},
    {"Page Fault", "#PF", true},
    {"Reserved", "-", true},
    {"x87 FPU Error", "#MF", true},
    {"Alignment Check", "#AC", true},
    {"Machine Check", "#MC", true},
    {"SIMD Floating-Point Exception", "#XM/#XF", true},
    {"Virtualization Exception", "#VE", true},
    {"Control Protection Exception", "#CP", true},
    {"Reserved", "-", true},
    {"Reserved", "-", true},
    {"Reserved", "-", true},
    {"Reserved", "-", true},
    {"Reserved", "-", true},
    {"Reserved", "-", true},
    {"Hypervisor Injection Exception", "#HV", true},
    {"VMM Communication Exception", "#VC", true},
    {"Security Exception", "#SX", true},
    {"Reserved", "-", true}};

void idt_set_gate(idt_entry_t idt[], int num, u64 base, u16 sel, u8 flags)
{
    idt[num].offsetLow = (base & 0xFFFF);
    idt[num].offsetMiddle = (base >> 16) & 0xFFFF;
    idt[num].offsetHigh = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].zero = 0;
}

int idt_init()
{
    idt_pointer.limit = sizeof(idt_entry_t) * IDT_ENTRY_COUNT - 1;
    idt_pointer.base = (uptr)&idt_entries;

    for (size_t i = 0; i < IRQ_COUNT; i++)
    {
        irq_handlers[i] = NULL;
    }

    for (int i = 0; i < 32; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
    }

    for (int i = IRQ_BASE; i < IRQ_BASE + IRQ_COUNT; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
    }

    idt_set_gate(idt_entries, 0x80, isr_table[0x80], 0x08, 0xEE);

    pic_disable();
    idt_load((u64)&idt_pointer);
    pic_enable();

    _panicked = false;
    DEBUG("interrupt", "IDT loaded with base=0x%lx, limit=%u", idt_pointer.base, idt_pointer.limit);
    return 0;
}

// TODO: Move
typedef struct stack_frame
{
    struct stack_frame *rbp;
    u64 rip;
} stack_frame_t;

void _backtrace(u64 caller)
{
    stack_frame_t *frame;
    asm volatile("mov %%rbp, %0" : "=r"(frame));

    ERROR("interrupt", "--- BACKTRACE START ----");

    if (frame && frame->rbp == NULL)
    {
        ERROR("interrupt", "No stack trace available-");
        return;
    }

    if (caller != 0)
    {
        ERROR("interrupt", " - [caller] %-24s @ %.16llx", "<unkown>", caller);
    }

    int offset = 0;
    for (int i = 0; frame && frame->rbp != NULL; i++)
    {
        ERROR("interrupt", " - [%06d] %-24s @ %.16llx", i - offset, "<unkown>", frame->rip);
        frame = frame->rbp;
    }

    ERROR("interrupt", "-----------------------");
}

void idt_handler(int_frame_t frame)
{
    if (frame.vector < 32) // Check for exceptions
    {
        DEBUG("interrupt", "Received exception %d (%s) on proc %s, %s", frame.vector, exception_info[frame.vector].mnemonic, scheduler_get_current_process()->name, exception_info[frame.vector].message);

        if (frame.vector == 1)
        {
            DEBUG("interrupt", "DEBUG @ 0x%.16llx", frame.rip);
#if _DEBUG
            hcf();
#endif // _DEBUG
        }

        if (exception_info[frame.vector].fatal)
        {
            if (_panicked)
            {
                ERROR("interrupt", "Recived a faulty exceptions whilst in another panic. Halting system.");
                hcf();
            }

            _panicked = true;
            ERROR("interrupt", "--- INTTERUPT INFORMATION ---");
            ERROR("interrupt", " - %s (%s) @ 0x%.16llx on CPU %s", exception_info[frame.vector].mnemonic, exception_info[frame.vector].message, frame.rip, "<unknown>");
            ERROR("interrupt", " - Register Dump:");
            ERROR("interrupt", "\tRAX: 0x%.16llx  RBX:    0x%.16llx  RCX: 0x%.16llx", frame.rax, frame.rbx, frame.rcx);
            ERROR("interrupt", "\tRDX: 0x%.16llx  RSI:    0x%.16llx  RDI: 0x%.16llx", frame.rdx, frame.rsi, frame.rdi);
            ERROR("interrupt", "\tRBP: 0x%.16llx  RSP:    0x%.16llx  -----------------------", frame.rbp, frame.rsp);
            ERROR("interrupt", "\tR8:  0x%.16llx  R9:     0x%.16llx  R10: 0x%.16llx", frame.r8, frame.r9, frame.r10);
            ERROR("interrupt", "\tR11: 0x%.16llx  R12:    0x%.16llx  R13: 0x%.16llx", frame.r11, frame.r12, frame.r13);
            ERROR("interrupt", "\tR14: 0x%.16llx  R15:    0x%.16llx  -----------------------", frame.r14, frame.r15);
            ERROR("interrupt", "\tRIP: 0x%.16llx  RFLAGS: 0x%.16llx  -----------------------", frame.rip, frame.rflags);
            ERROR("interrupt", "\tCR2: 0x%.16llx  CR3:    0x%.16llx  -----------------------", frame.cr2, frame.cr3);
            ERROR("interrupt", "\tCS:  0x%.16llx  SS:     0x%.16llx  -----------------------", frame.cs, frame.ss);
            ERROR("interrupt", "\tERR: 0x%.16llx  VECTOR: 0x%.16llx  -----------------------", frame.err, frame.vector);

            switch (frame.vector)
            {
            case 8:
                ERROR("interrupt", " - Double Fault: No additional error information.");
                break;
            case 10:
                ERROR("interrupt", " - Invalid TSS Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                ERROR("interrupt", "\t - Index: 0x%.16llx", (frame.err >> 3) & 0x1FFF);
                ERROR("interrupt", "\t - %s", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 11:
                ERROR("interrupt", " - Segment Not Present Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                ERROR("interrupt", "\t - Index: 0x%.16llx", (frame.err >> 3) & 0x1FFF);
                ERROR("interrupt", "\t - %s", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 12:
                ERROR("interrupt", " - Stack-Segment Fault Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                ERROR("interrupt", "\t - Index: 0x%.16llx", (frame.err >> 3) & 0x1FFF);
                ERROR("interrupt", "\t - %s", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 13:
                ERROR("interrupt", " - General Protection Fault Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                ERROR("interrupt", "\t - Index: 0x%.16llx", (frame.err >> 3) & 0x1FFF);
                ERROR("interrupt", "\t - %s", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 14:
                ERROR("interrupt", " - Page Fault Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "Protection violation" : "Non-present page");
                ERROR("interrupt", "\t - %s", (frame.err & 2) ? "Write access" : "Read access");
                ERROR("interrupt", "\t - %s", (frame.err & 4) ? "User mode" : "Supervisor mode");
                ERROR("interrupt", "\t - %s", (frame.err & 8) ? "Reserved bit set" : "No reserved bit violation");
                ERROR("interrupt", "\t - %s", (frame.err & 16) ? "Instruction fetch" : "Data access");
                break;
            case 17:
                ERROR("interrupt", " - Alignment Check Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                ERROR("interrupt", "\t - %s", (frame.err & 1) ? "User mode" : "Supervisor mode");
                break;
            case 21:
                ERROR("interrupt", " - Control Protection Exception Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                break;
            case 29:
                ERROR("interrupt", " - VMM Communication Exception Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                break;
            case 30:
                ERROR("interrupt", " - Security Exception Details:");
                ERROR("interrupt", "\tError Code: 0x%.16llx", frame.err);
                break;
            default:
                ERROR("interrupt", " - No detailed information for this exception.");
                break;
            }

            ERROR("interrupt", "-------------------------------");
            pmm_dump();
            //_backtrace(frame.rip);
            hcf();
        }
        else
        {
            switch (frame.vector)
            {
            case 3:
                NOTE("interrupt", "BREAKPOINT @ 0x%p", frame.rip);
                break;
            default:
                WARN("interrupt", "Non-fatal exception: %s", exception_info[frame.vector].message);
                break;
            }
        }
    }
    else if (frame.vector >= IRQ_BASE && frame.vector < IRQ_BASE + IRQ_COUNT)
    {
        int irq = frame.vector - IRQ_BASE;
        // DEBUG("interrupt", "Received IRQ %d", irq);
        if (irq_handlers[irq])
        {
            // DEBUG("interrupt", "Invoking handler for IRQ %d", irq);
            irq_handlers[irq](&frame);
            // DEBUG("interrupt", "Executed IRQ handler for IRQ %d", irq);
        }
        else
        {
            WARN("interrupt", "No handler registered for IRQ %d, interrupt ignored", irq);
        }

        pic_send_end_of_interrupt(irq);
    }
    else if (frame.vector == 0x80) // System call
    {
        // DEBUG("interrupt", "Received system call from proc %s (syscall: %llu)", scheduler_get_current_process()->name, frame.rax);
        u64 result = syscall_handler(&frame);
        // frame.rax = result;
        (void)result;
    }
    else // Unknown interrupt
    {
        WARN("interrupt", "Received unknown interrupt vector: %d, no action taken", frame.vector);
    }
}

void idt_irq_register(int irq, idt_handler_t handler)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("interrupt", "Attempted to register invalid IRQ: %d", irq);
        return;
    }
    irq_handlers[irq] = handler;
    DEBUG("interrupt", "Registered IRQ handler for IRQ %d", irq);
}

void idt_irq_deregister(int irq)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("interrupt", "Attempted to deregister invalid IRQ: %d", irq);
        return;
    }
    irq_handlers[irq] = 0;
    DEBUG("interrupt", "Deregistered IRQ handler for IRQ %d", irq);
}