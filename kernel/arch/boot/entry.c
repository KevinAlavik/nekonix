#include <limine.h>
#include <lib/string.h>
#include <sys/portio.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};

__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf(void)
{
    __asm__ volatile("cli");
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}

void puts(const char *str)
{
    while (*str)
        outb(0xE9, *str++);
}

void kmain(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        hcf();
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    uint32_t color = 0xFFFFFF;

    for (size_t i = 0; i < 100; i++)
    {
        volatile uint32_t *fb_ptr = framebuffer->address;
        memset((void *)&fb_ptr[i * (framebuffer->pitch / 4) + i], color, sizeof(uint32_t));
    }

    puts("Hello, Nekonix v1.0.0-alpha!\n");

    hcf();
}