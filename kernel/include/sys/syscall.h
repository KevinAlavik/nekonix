#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/idt.h>
#include <lib/types.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6);
u64 syscall_handler(int_frame_t *frame);

#endif // SYSCALL_H