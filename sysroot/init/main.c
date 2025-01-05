#include <stdio.h>
#include <syscall.h>

int main()
{
    printf("Hello, World!\n");
    while (1)
    {
        u8 data = 0;
        if (read(1, &data) == 0)
        {
            printf("Error: Failed to read from stdin.\n");
            return 1;
        }

        if (data != 0x0)
            printf("Received: 0x%x\n", data);
    }

    return 69;
}