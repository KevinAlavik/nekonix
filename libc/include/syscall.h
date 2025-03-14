#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <types.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6);
void write(u32 id, void *data, usize size);
u64 read(u32 id, void *out);
u8 poll(u32 id);

#endif // _SYSCALL_H