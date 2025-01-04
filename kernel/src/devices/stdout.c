#include <devices/stdout.h>

handle_t stdout_handle;

static u8 stdout_poll()
{
    return DEVICE_READY;
}

static u64 stdout_read(void *buf)
{
    (void)buf;
    return 0;
}

static void stdout_write(void *buf, usize count)
{
    for (size_t i = 0; i < count; i++)
    {
        // use flanterm_putchar hardcoded instead of putchar_impl.
        flanterm_putchar(((char *)buf)[i]);
    }
}

void stdout_init()
{

    handle_t _handle = {
        .id = 0,
        .poll = stdout_poll,
        .read = stdout_read,
        .write = stdout_write};

    stdout_handle = _handle;
}