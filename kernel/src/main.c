#include <boot/limine.h>
#include <dev/serial_util.h>
#include <core/cpu.h>
#include <lib/stdio.h>
#include <boot/nnix.h>
#include <stdbool.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;

int serial_putchar(char c)
{
    outb(_stdout_port, c);
    if (c == '\n')
    {
        outb(_stdout_port, '\r');
    }
    return c;
}

int flanterm_putchar(char c)
{
    flanterm_write(ft_ctx, &c, 1);
    return c;
}

void sys_entry(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        outstr(0xE9, "ERROR: Limine Base Revision is not supported.\n");
        hcf();
    }

    if (init_serial(0x3F8) != 0)
    {
        outstr(0xE9, "ERROR: Failed to initialize COM1, looking for alternative outputs.\n");
        u16 new_port = serial_get_new();
        _stdout_port = new_port;
        printf("INFO: Selected 0x%.2X as new stdout port.\n", _stdout_port);
    }

    putchar_impl = serial_putchar;

    bool no_fb = false;
    if (!framebuffer_request.response)
    {
        printf("WARNING: Failed to get framebuffer.\n");
        no_fb = true;
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    if (!framebuffer)
    {
        printf("WARNING: No framebuffers availible.\n");
        no_fb = true;
    };

    if (!no_fb)
    {

        ft_ctx = flanterm_fb_init(
            NULL,
            NULL,
            framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
            framebuffer->red_mask_size, framebuffer->red_mask_shift,
            framebuffer->green_mask_size, framebuffer->green_mask_shift,
            framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
            NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, NULL,
            NULL, 0, 0, 1,
            0, 0,
            0);

        if (!ft_ctx)
        {
            printf("WARNING: Failed to initialize flanterm.\n");
        }
        else
        {
            ft_ctx->cursor_enabled = false;
            ft_ctx->full_refresh(ft_ctx);

            putchar_impl = flanterm_putchar;
            printf("Logging enabled on serial port 0x%.2X\n", _stdout_port);
        }
    }

    // TODO: Make a proper stream instead of manualy changing putchar impl
    putchar_impl = serial_putchar;
    printf("NNix (Nikonix) v%s.%s.%s%s (%dx%d)\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE, framebuffer->width, framebuffer->height);

    hlt();
}
