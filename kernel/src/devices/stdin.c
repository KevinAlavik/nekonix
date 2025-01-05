#include <devices/stdin.h>
#include <sys/cpu.h>
#include <dev/serial.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <lib/string.h>
#include <stdbool.h>

#define PS2_DATA 0x60

handle_t stdin_handle;

static key_t key = {0};

// Keymap for translating scancodes
static const char *keymap[] = {
    [0x01] = "Esc", [0x02] = "1", [0x03] = "2", [0x04] = "3", [0x05] = "4", [0x06] = "5", [0x07] = "6", [0x08] = "7", [0x09] = "8", [0x0A] = "9", [0x0B] = "0", [0x0C] = "-", [0x0D] = "=", [0x0E] = "Backspace", [0x0F] = "Tab", [0x10] = "Q", [0x11] = "W", [0x12] = "E", [0x13] = "R", [0x14] = "T", [0x15] = "Y", [0x16] = "U", [0x17] = "I", [0x18] = "O", [0x19] = "P", [0x1C] = "Enter", [0x1D] = "Ctrl", [0x1E] = "A", [0x1F] = "S", [0x20] = "D", [0x21] = "F", [0x22] = "G", [0x23] = "H", [0x24] = "J", [0x25] = "K", [0x26] = "L", [0x27] = ";", [0x29] = "`", [0x2A] = "Shift", [0x2C] = "Z", [0x2D] = "X", [0x2E] = "C", [0x2F] = "V", [0x30] = "B", [0x31] = "N", [0x32] = "M", [0x33] = ",", [0x34] = ".", [0x35] = "/", [0x39] = "Space", [0x3A] = "CapsLock", [0x3B] = "F1", [0x57] = "F11", [0x58] = "F12"};

// Keyboard interrupt handler
static void keyboard_handler(int_frame_t *frame)
{
    (void)frame;
    u8 data = inb(PS2_DATA);
    bool is_released = (data & 0x80) != 0;
    u8 scancode = data & 0x7F;

    key.sym = keymap[scancode];
    key.scancode = scancode;
    key.released = is_released;

    pic_send_end_of_interrupt(1);
}

// Poll function
static u8 stdin_poll()
{
    return key.sym ? DEVICE_READY : DEVICE_BUSY;
}

// Read function
static u64 stdin_read(void *buf)
{
    if (key.sym)
    {
        *(key_t *)buf = key;
        key.sym = NULL; // Clear key after reading
        return 1;       // Return one key
    }
    return 0; // No new key
}

// Write function (stub)
static void stdin_write(void *buf, usize count)
{
    (void)buf;
    (void)count;
}

// Initialize stdin
void stdin_init()
{
    stdin_handle = (handle_t){
        .id = 1,
        .poll = stdin_poll,
        .read = stdin_read,
        .write = stdin_write};
    idt_irq_register(1, keyboard_handler);
}
