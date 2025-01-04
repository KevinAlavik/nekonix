#include <syscall.h>
#include <types.h>

int main()
{
    write(0, "Hello, World!\n", 14);
    return 69;
}