#include <sys/syscall.h>
#include <boot/nnix.h>
#include <proc/scheduler.h>
#include <proc/manager.h>
#include <mm/vmm.h>

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

    case 2: // write(id, data, size)
        handle_t *device = device_get(frame->rdi);
        if (device == NULL)
            return 0;

        while (device->poll() == DEVICE_BUSY)
            ;
        device->write((void *)frame->rsi, frame->rdx);
        return 0;
    case 3: // read(id, out)
        device = device_get(frame->rdi);
        if (device == NULL)
            return 0;

        while (device->poll() == DEVICE_BUSY)
            ;

        return device->read((void *)frame->rsi);
    default:
        ERROR("syscall", "Unknown system call %d", frame->rax);
        return 0;
    }
}