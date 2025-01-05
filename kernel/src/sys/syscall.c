#include <sys/syscall.h>
#include <boot/nnix.h>
#include <proc/scheduler.h>
#include <proc/manager.h>
#include <mm/vmm.h>
#include <lib/string.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6)
{
    u64 result;
    __asm__ volatile(
        "movq %1, %%rax\n\t"
        "movq %2, %%rdi\n\t"
        "movq %3, %%rsi\n\t"
        "movq %4, %%rdx\n\t"
        "movq %5, %%rcx\n\t"
        "movq %6, %%r8\n\t"
        "movq %7, %%r9\n\t"
        "int $0x80\n\t"
        "movq %%rax, %0"
        : "=r"(result)
        : "r"(number), "r"(arg1), "r"(arg2), "r"(arg3),
          "r"(arg4), "r"(arg5), "r"(arg6)
        : "rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "memory");

    return result;
}

u64 syscall_handler(int_frame_t *frame)
{
    // Number: rax
    // Arguments:
    // - rdi: arg1
    // - rsi: arg2
    // - rdx: arg3
    // - rcx: arg4
    // - r8: arg5
    // - r9: arg6

    handle_t *device; // current device.

    switch (frame->rax)
    {
    case 1: // exit()
        scheduler_terminate_current_process(frame->rdi);
        return 0;

    case 2: // write(id, data, size)
        device = device_get(frame->rdi);
        if (device == NULL)
            return 0;

        while (device->poll() == DEVICE_BUSY)
            scheduler_context_switch(frame);

        device->write((void *)frame->rsi, frame->rdx);
        return 0;
    case 3: // read(id, out)
        device = device_get(frame->rdi);
        if (device == NULL)
            return 0;

        u64 result = device->read((void *)frame->rsi);
        frame->rax = result;
        return result;
    case 4: // poll(id)
        device = device_get(frame->rdi);
        return device->poll();

    default:
        ERROR("syscall", "Unknown system call %d", frame->rax);
        return 0;
    }
}