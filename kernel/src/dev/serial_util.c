#include <dev/serial_util.h>

void outstr(u16 port, const char *str)
{
    while (*str)
    {
        outb(port, (u8)*str++);
    }
}