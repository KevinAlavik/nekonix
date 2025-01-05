#include <devices/stdin.h>
#include <sys/cpu.h>
#include <dev/serial.h>
#include <sys/idt.h>
#include <sys/pic.h>

handle_t stdin_handle;
bool has_input = false;
u8 last_data = 0;

#define PS2_COMMAND 0x64
#define PS2_DATA 0x60

void keyboard_handler(int_frame_t *frame)
{
    (void)frame; // Ignoring frame as it is not used here
    u8 data = inb(PS2_DATA);

    if (last_data != data)
    {
        last_data = data;
        has_input = true;
    }

    DEBUG("keyboard", "Received keyboard data: 0x%x", data);

    pic_send_end_of_interrupt(1); // Acknowledge interrupt to the PIC
}

static u8 stdin_poll()
{
    // Check if the input is ready to be read
    return has_input ? DEVICE_READY : DEVICE_BUSY;
}

static u64 stdin_read(void *buf)
{
    // Only allow reading if there's input
    if (has_input)
    {
        *((u8 *)buf) = last_data;
        has_input = false; // Reset flag after reading
        return 1;
    }

    return 0; // No data available to read
}

static void stdin_write(void *buf, usize count)
{
    (void)buf;
    (void)count;
    // stdin typically wouldn't handle writes, so we leave this as is
}

void stdin_init()
{
    handle_t _handle = {
        .id = 1,
        .poll = stdin_poll,
        .read = stdin_read,
        .write = stdin_write};

    stdin_handle = _handle;

    // Register the keyboard interrupt handler (IRQ 1)
    idt_irq_register(1, keyboard_handler);
}
