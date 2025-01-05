#include <syscall.h>

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

void write(u32 id, void *data, usize size)
{
  syscall(2, id, (u64)data, size, 0, 0, 0);
}

u64 read(u32 id, void *out)
{
  return syscall(3, id, (u64)out, 0, 0, 0, 0);
}

u8 poll(u32 id)
{
  return (u8)syscall(4, id, 0, 0, 0, 0, 0);
}