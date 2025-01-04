#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/idt.h>
#include <lib/types.h>

u64 syscall_handler(int_frame_t *frame);

#endif // SYSCALL_H