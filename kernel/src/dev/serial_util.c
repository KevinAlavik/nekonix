#include <dev/serial_util.h>

void outstr(u16 port, const char *str)
{
    while (*str)
    {
        outb(port, (u8)*str);
        if (*str == '\n')
        {
            outb(0x3F8, '\r');
        }

        str++;
    }
}