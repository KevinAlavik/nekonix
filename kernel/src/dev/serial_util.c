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
    /*
        COM1 	0x3F8 (note: should already been tested prior to this)
        COM2 	0x2F8
        COM3 	0x3E8
        COM4 	0x2E8
        COM5 	0x5F8
        COM6 	0x4F8
        COM7 	0x5E8
        COM8 	0x4E8
    */

    u16 com_ports[] = {
        0x3f8,
        0x2f8,
        0x3e8,
        0x2e8,
        0x5f8,
        0x4f8,
        0x5e8,
        0x4e8};

    for (u32 i = 0; i < sizeof(com_ports) / sizeof(com_ports[0]); i++)
    {
        if (init_serial(com_ports[i]) != 1)
        {
            return com_ports[i];
        }
    }

    return 0xE9;
}