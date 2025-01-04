#include <sys/syscall.h>
#include <boot/nnix.h>
#include <proc/scheduler.h>

u64 syscall_handler(int_frame_t *frame)
{
    // Arguments:
    // - rdi: arg1
    // - rsi: arg2
    // - rdx: arg3
    // - rcx: arg4
    // - r8: arg5
    // - r9: arg6

    switch (frame->rax)
    {
    case 1: // exit()
        scheduler_terminate_current_process(frame->rdi);
        return 0;
    case 2: // putchar(), test
        return flanterm_putchar(frame->rdi);
    default:
        ERROR("syscall", "Unknown system call %d", frame->rax);
        return 0;
    }
}