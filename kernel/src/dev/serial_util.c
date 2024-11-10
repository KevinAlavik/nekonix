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

int init_serial(u16 port)
{
    outb(port + 1, 0x00); // Disable all interrupts
    outb(port + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00); //                  (hi byte)
    outb(port + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B); // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E); // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE); // Test serial chip (send byte 0xAE and check if serial returns same byte)

    if (inb(port + 0) != 0xAE)
    {
        return 1;
    }

    outb(port + 4, 0x0F);
    return 0;
}

u16 serial_get_new()
{
    return 0xE9;
}