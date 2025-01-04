unsigned long syscall(unsigned long number, unsigned long arg1, unsigned long arg2,
                      unsigned long arg3, unsigned long arg4, unsigned long arg5,
                      unsigned long arg6)
{
    unsigned long result;

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

void puts(const char *str)
{
    while (*str)
    {
        syscall(2, *str++, 0, 0, 0, 0, 0);
    }
}

int main()
{
    puts("Hello Nekonix >:D\n");
    return 69;
}