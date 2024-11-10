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
    _stdout_port = 0xE9;
    putchar_impl = serial_putchar;
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        ERROR("boot", "Limine Base Revision is not supported.");
        hcf();
    }

    _stdout_port = serial_get_new();

    bool no_fb = false;
    if (!framebuffer_request.response)
    {
        WARN("boot", "Failed to get framebuffer.");
        no_fb = true;
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    if (!framebuffer)
    {
        WARN("boot", "No framebuffers availible.");
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
            WARN("boot", "Failed to initialize Flanterm.");
        }
        else
        {
            ft_ctx->cursor_enabled = false;
            ft_ctx->full_refresh(ft_ctx);

            putchar_impl = flanterm_putchar;
            if (!_GRAPHICAL_LOG)
            {
                WARN("boot", "Graphical text output is disabled, refer to the kernel config. (%d)", _GRAPHICAL_LOG);
                INFO("boot", "Logging is enabled on serial port: 0x%.2X", _stdout_port);
            }
        }
    }

    // TODO: Make a proper stream instead of manualy changing putchar impl
    putchar_impl = _GRAPHICAL_LOG ? flanterm_putchar : serial_putchar;
    INFO("testing", "NNix (Nikonix) v%s.%s.%s%s (%dx%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE, framebuffer->width, framebuffer->height);

    hlt();
}
