#include <syscall.h>

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