#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (size_t i = 0; i < n; i++)
    {
        d[i] = s[i];
    }
    return dest;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    for (size_t i = 0; i < n; i++)
    {
        p[i] = (uint8_t)c;
    }
    return s;
}

void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    if (s > d)
    {
        for (size_t i = 0; i < n; i++)
        {
            d[i] = s[i];
        }
    }
    else if (s < d)
    {
        for (size_t i = n; i > 0; i--)
        {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1;
    const uint8_t *p2 = s2;
    for (size_t i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
    return 0;
}

static void hcf(void)
{
    for (;;)
    {
        asm volatile("hlt");
    }
}

void sys_entry(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        hcf();
    }
    if (!framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    volatile uint32_t *fb_ptr = (uint32_t *)fb->address;
    size_t pitch = fb->pitch / 4;
    for (size_t i = 0; i < 100 && i < fb->width; i++)
    {
        fb_ptr[i * pitch + i] = 0xFFFFFF;
    }

    for (;;)
    {
        asm volatile("hlt");
    }
}
