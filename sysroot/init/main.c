#include <stdio.h>
#include <syscall.h>

int main()
{
    printf("Hello, World! try typing :^)\n");
    while (1)
    {
        key_t key;
        while (poll(1) == DEVICE_BUSY)
            ;
        u64 status = read(1, &key);

        if (status != 0 && key.released == false && key.sym)
        {
            if (key.scancode == KEYCODE_SPACE)
            {
                printf(" ");
            }
            else if (key.scancode == KEYCODE_ENTER)
            {
                printf("\n");
            }
            else if (key.scancode == KEYCODE_BACKSPACE)
            {
                printf("\b \b");
            }
            else
            {
                printf("%s", key.sym);
            }
        }
    }

    return 69;
}