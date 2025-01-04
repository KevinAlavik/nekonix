#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <types.h>

u64 syscall(u64 number, u64 arg1, u64 arg2, u64 arg3, u64 arg4, u64 arg5, u64 arg6);

#endif // _SYSCALL_H