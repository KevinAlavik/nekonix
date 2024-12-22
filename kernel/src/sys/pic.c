#include <sys/pic.h>
#include <dev/serial.h>
#include <dev/serial_util.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_ICW1_ICW4 0x01
#define PIC_ICW1_INITIALIZE 0x10
#define PIC_ICW4_8086 0x01
#define PIC_ICW4_AUTO_EOI 0x02
#define PIC_CMD_END_OF_INTERRUPT 0x20
#define PIC_CMD_READ_IRR 0x0A
#define PIC_CMD_READ_ISR 0x0B

u16 _pic_mask = 0xFFFF;

void pic_set_mask(u16 mask)
{
    _pic_mask = mask;
    outb(PIC1_DATA, _pic_mask & 0xFF);
    io_wait();
    outb(PIC2_DATA, _pic_mask >> 8);
    io_wait();
}

u16 pic_get_mask()
{
    return inb(PIC1_DATA) | (inb(PIC2_DATA) << 8);
}

void pic_configure(u8 offset_pic1, u8 offset_pic2, bool auto_eoi)
{
    pic_set_mask(0xFFFF);

    outb(PIC1_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, offset_pic1);
    io_wait();
    outb(PIC2_DATA, offset_pic2);
    io_wait();

    outb(PIC1_DATA, 0x04); // PIC1 has a slave PIC2 at IRQ2
    io_wait();
    outb(PIC2_DATA, 0x02); // PIC2 is connected to PIC1
    io_wait();

    u8 icw4 = PIC_ICW4_8086;
    if (auto_eoi)
    {
        icw4 |= PIC_ICW4_AUTO_EOI;
    }

    outb(PIC1_DATA, icw4);
    io_wait();
    outb(PIC2_DATA, icw4);
    io_wait();

    pic_set_mask(0xFFFF); // Mask all IRQs initially
}

void pic_send_end_of_interrupt(int irq)
{
    if (irq >= 8)
    {
        outb(PIC2_COMMAND, PIC_CMD_END_OF_INTERRUPT);
        io_wait();
    }
    outb(PIC1_COMMAND, PIC_CMD_END_OF_INTERRUPT);
    io_wait();
}

void pic_disable()
{
    pic_set_mask(0xFFFF); // Mask all IRQs
}

void pic_enable()
{
    pic_configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8, false);
    pic_set_mask(0xFFFF);
}

void pic_mask(int irq)
{
    pic_set_mask(_pic_mask | (1 << irq));
}

void pic_unmask(int irq)
{
    pic_set_mask(_pic_mask & ~(1 << irq));
}

u16 pic_read_irq_request_register()
{
    outb(PIC1_COMMAND, PIC_CMD_READ_IRR);
    io_wait();
    u16 irr = inb(PIC1_DATA);
    outb(PIC2_COMMAND, PIC_CMD_READ_IRR);
    io_wait();
    irr |= ((u16)inb(PIC2_DATA)) << 8;
    return irr;
}

u16 pic_read_in_service_register()
{
    outb(PIC1_COMMAND, PIC_CMD_READ_ISR);
    io_wait();
    u16 isr = inb(PIC1_DATA);
    outb(PIC2_COMMAND, PIC_CMD_READ_ISR);
    io_wait();
    isr |= ((u16)inb(PIC2_DATA)) << 8;
    return isr;
}
