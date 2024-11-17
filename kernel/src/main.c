#include <boot/limine.h>
#include <dev/serial_util.h>
#include <core/cpu.h>
#include <lib/stdio.h>
#include <boot/nnix.h>
#include <stdbool.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <core/gdt.h>
#include <core/idt.h>
#include <mm/pmm.h>
#include <lib/string.h>

u64 hhdm_offset;

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
};
__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

static struct flanterm_context *ft_ctx;
static bool no_fb = false;

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

int mirror_putchar(char c)
{
    serial_putchar(c);
    flanterm_putchar(c);
    return c;
}

void graphics_init(void)
{
    if (!framebuffer_request.response)
    {
        WARN("boot", "Failed to get framebuffer.");
        no_fb = true;
        return;
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    if (!framebuffer)
    {
        WARN("boot", "No framebuffers available.");
        no_fb = true;
        return;
    }

    ft_ctx = flanterm_fb_init(
        NULL, NULL,
        framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0, 0);

    if (!ft_ctx)
    {
        WARN("boot", "Failed to initialize Flanterm.");
    }
    else
    {
        ft_ctx->cursor_enabled = false;
        ft_ctx->full_refresh(ft_ctx);
    }
}

void output_init(void)
{
    if (!no_fb)
    {
        putchar_impl = flanterm_putchar;
        if (!_GRAPHICAL_LOG)
        {
            WARN("boot", "Graphical text output is disabled, refer to the kernel config. (%d)", _GRAPHICAL_LOG);
            INFO("boot", "Logging is enabled on serial port: 0x%.2X", _stdout_port);
            putchar_impl = serial_putchar;
        }
    }
    else
    {
        putchar_impl = serial_putchar;
    }

    putchar_impl = _MIRROR_LOG || _ERROR_LOG ? mirror_putchar : putchar_impl;
}

void interrupts_init(void)
{
    int gdt_error_count = 0;
    if (gdt_init() != 0)
    {
        ERROR("boot", "Failed to initialize GDT, unkown error.");
        gdt_error_count++;
    }

    INFO("boot", "Initialized GDT (%d errors reported)", gdt_error_count);
    if (gdt_error_count > 0)
    {
        ERROR("boot", "Error(s) happened during initialization or testing of the GDT, this will result in a system halt. Bye!");
        hcf();
    }

    int idt_error_count = 0;
    if (idt_init() != 0)
    {
        ERROR("boot", "Failed to initialize IDT, unkown error.");
        idt_error_count++;
    }

    INFO("boot", "Initialized IDT (%d errors reported)", idt_error_count);
    if (idt_error_count > 0)
    {
        ERROR("boot", "Error(s) happened during initialization or testing of the IDT, this will result in a system halt. Bye!");
        hcf();
    }
}

int test_pmm(int tests)
{
    for (int i = 1; i < tests + 1; i++)
    {
        int *a = (int *)HIGHER_HALF(pmm_request_page());
        if (a == NULL)
        {
            DEBUG("boot", "Failed to allocate a single page for test %d", i);
            return 1;
        }

        //*a = 69;

        DEBUG("test", "(test %d) Allocated 1 page at: %p", i, (u64)a);
        pmm_free_page(PHYSICAL(a));
    }
    return 0;
}

void memory_init(void)
{
    hhdm_offset = hhdm_request.response->offset;
    int error_count = 0;

    DEBUG("boot", "HHDM Offset: 0x%.16llx", hhdm_offset);

    if (pmm_init(memmap_request.response) != 0)
    {
        ERROR("boot", "Failed to initialize PMM (Physical Memory Manager), unkown error");
        error_count++;
    }

    if (test_pmm(_PMM_TESTS) != 0)
    {
        ERROR("boot", "An error occurred during testing of PMM (Physical Memory Manager), unkown error");
        error_count++;
    }

    INFO("boot", "Initialized PMM (%d errors reported), %d tests ran", error_count, _PMM_TESTS);
    if (error_count > 0)
    {
        ERROR("boot", "Error(s) happened during initialization or testing for the PMM (Physical Memory Manager), this will result in a system halt. Bye!");
        hcf();
    }
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

    graphics_init();
    output_init();

    INFO("boot", "Nekonix (Nnix.) v%s.%s.%s%s (%dx%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE, framebuffer_request.response->framebuffers[0]->width, framebuffer_request.response->framebuffers[0]->height);

    interrupts_init();
    memory_init();

    printf("\n");
    printf("Nnix. v%s.%s.%s%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE);
    pmm_dump();

    hlt();
}
