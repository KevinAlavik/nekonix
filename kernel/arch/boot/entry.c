#include <limine.h>
#include <lib/string.h>
#include <sys/portio.h>
#include <utils/printf.h>
#include <sys/cpu.h>

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

int serial_putchar(char ch)
{
    outb(0xE9, ch);
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

    uint32_t color = 0xFFFFFF;

    for (size_t i = 0; i < 100; i++)
    {
        volatile uint32_t *fb_ptr = framebuffer->address;
        memset((void *)&fb_ptr[i * (framebuffer->pitch / 4) + i], color, sizeof(uint32_t));
    }

    if (kernel_file_request.response == NULL)
    {
        printf("ERROR: No kernel file found\n");
        hcf();
    }

    struct limine_file *file = kernel_file_request.response->executable_file;

    printf("%s\n", file->cmdline);

    hcf();
}