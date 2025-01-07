#include <limine.h>
#include <nnix.h>
#include <lib/string.h>
#include <sys/portio.h>
#include <utils/printf.h>
#include <sys/cpu.h>
#include <utils/log.h>
#include <utils/flanterm/flanterm.h>
#include <utils/flanterm/backends/fb.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};
__attribute__((section(".limine_requests"))) static volatile struct limine_executable_file_request kernel_file_request = {
    .id = LIMINE_EXECUTABLE_FILE_REQUEST,
    .revision = 0,
    .response = NULL};
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;
bool kernel_debug_enabled = false;
struct cmdline_arg kernel_args[10];
int kernel_arg_count = 0;

int serial_putchar(char ch)
{
    outb(0xE9, ch);
    return ch;
}

int flanterm_putchar(char ch)
{
    flanterm_write(ft_ctx, &ch, 1);
    return ch;
}

void kmain(void)
{
    putchar_impl = serial_putchar;
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        printf("ERROR: LIMINE base revision not supported\n");
        hcf();
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        printf("ERROR: No framebuffer found\n");
        hcf();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    if (kernel_file_request.response == NULL)
    {
        printf("ERROR: No kernel file found\n");
        hcf();
    }

    ft_ctx = flanterm_fb_init(
        NULL, NULL,
        framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0, 0);

    if (ft_ctx == NULL)
    {
        ERROR("Failed to initialize flanterm, falling back to 0xE9 serial output");
        putchar_impl = serial_putchar;
    }
    else
    {
        ft_ctx->cursor_enabled = false;
        ft_ctx->full_refresh(ft_ctx);
        putchar_impl = flanterm_putchar;
    }

    struct limine_file *kernel_file = kernel_file_request.response->executable_file;
    parse_cmdline(kernel_file->cmdline, kernel_args);
    while (kernel_args[kernel_arg_count].name != NULL)
    {
        kernel_arg_count++;
    }

    if (cmdline_has_arg("debug") && cmdline_get_bool("debug", false))
    {
        kernel_debug_enabled = true;
    }

    INFO("Nekonix v%s.%s.%s%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_EXTRA);

    INFO("Kernel started successfully");

    hcf();
}
