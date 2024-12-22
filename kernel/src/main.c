#include <boot/limine.h>
#include <dev/serial_util.h>
#include <sys/cpu.h>
#include <lib/stdio.h>
#include <boot/nnix.h>
#include <stdbool.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <mm/pmm.h>
#include <lib/string.h>
#include <mm/vmm.h>
#include <mm/vma.h>
#include <mm/liballoc/liballoc.h>
#include <dev/vfs.h>
#include <fs/ramfs.h>
#include <dev/rtc.h>
#include <dev/hvfs.h>

#define _PMM_TESTS 10
#define _VMM_TESTS 10

u64 hhdm_offset;
u64 __kernel_phys_base;
u64 __kernel_virt_base;
void *__kernel_vma_context;

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
__attribute__((used, section(".limine_requests"))) static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests"))) static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;

// Output callbacks.
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

// Memory management tests.
int test_pmm(int tests)
{
    for (int i = 1; i < tests + 1; i++)
    {
        int *a = (int *)HIGHER_HALF(pmm_request_page());
        if (a == NULL)
        {
            ERROR("test-pmm", "Failed to allocate a single page for test %d", i);
            return 1;
        }

        *a = 69;
        if (*a != 69 || a == NULL)
        {
            DEBUG("test-pmm", "test %d failed to write to 1 page at: 0x%.16llx", i, (u64)a);
            pmm_free_page(PHYSICAL(a));
            return 1;
        }

        DEBUG("test-pmm", "test %d successfully allocated and wrote to 1 page at: 0x%.16llx", i, (u64)a);
        pmm_free_page(PHYSICAL(a));
    }
    return 0;
}

int test_vmm(int tests, vma_context_t *ctx)
{
    for (int i = 1; i < tests + 1; i++)
    {
        int *a = (int *)vma_alloc(ctx, 1, VMM_PRESENT | VMM_WRITABLE);
        if (a == NULL)
        {
            ERROR("test-vmm", "Failed to allocate a single page for test %d", i);
            vma_destroy_context(ctx);
            return 1;
        }

        *a = 69;
        if (*a != 69)
        {
            DEBUG("test-vmm", "test %d failed to write to 1 page at: 0x%.16llx", i, (u64)a);
            vma_free(ctx, a);
            vma_destroy_context(ctx);
            return 1;
        }

        DEBUG("test-vmm", "test %d successfully allocated and wrote to 1 page at: 0x%.16llx", i, (u64)a);
        vma_free(ctx, a);
    }
    return 0;
}

// Kernel entry point.
extern void test();
void sys_entry(void)
{
    // Serial output setup
    _stdout_port = 0xE9;
    putchar_impl = serial_putchar;

    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        ERROR("boot", "Limine Base Revision is not supported.");
        hcf();
    }

    _stdout_port = serial_get_new();

    // Graphics initialization
    if (!framebuffer_request.response)
    {
        WARN("boot", "Failed to get framebuffer.");
        hcf();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    if (!framebuffer)
    {
        WARN("boot", "No framebuffers available.");
        hcf();
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
        hcf();
    }
    else
    {
        ft_ctx->cursor_enabled = false;
        ft_ctx->full_refresh(ft_ctx);
    }

    putchar_impl = serial_putchar;

    // Time initialization
    rtc_init();

    // Display ASCII art
    char *_text[] = {
        " _   _       _        ",
        "| \\ | |_ __ (_)_  __  ",
        "|  \\| | '_ \\| \\ \\/ /  ",
        "| |\\  | | | | |>  < _ ",
        "|_| \\_|_| |_|_/_/\\_(_)",
        "                      ",
        NULL};

    for (int i = 0; _text[i] != NULL; i++)
    {
        printf("%s\n", _text[i]);
    }

    // Display kernel version
    printf("(Nekonix v%s.%s.%s%s)\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE);

    // Interrupts initialization
    int gdt_error_count = 0;
    if (gdt_init() != 0)
    {
        ERROR("boot", "Failed to initialize GDT, unknown error.");
        gdt_error_count++;
    }

    INFO("boot", "Initialized GDT (%d errors reported)", gdt_error_count);
    if (gdt_error_count > 0)
    {
        ERROR("boot", "Error(s) happened during initialization of the GDT. Halting.");
        hcf();
    }

    int idt_error_count = 0;
    if (idt_init() != 0)
    {
        ERROR("boot", "Failed to initialize IDT, unknown error.");
        idt_error_count++;
    }

    INFO("boot", "Initialized IDT (%d errors reported)", idt_error_count);
    if (idt_error_count > 0)
    {
        ERROR("boot", "Error(s) happened during initialization of the IDT. Halting.");
        hcf();
    }

    // Memory initialization
    hhdm_offset = hhdm_request.response->offset;
    int pmm_error_count = 0;

    DEBUG("boot", "HHDM Offset: 0x%.16llx", hhdm_offset);

    if (pmm_init(memmap_request.response) != 0)
    {
        ERROR("boot", "Failed to initialize PMM, unknown error.");
        pmm_error_count++;
    }

    if (test_pmm(_PMM_TESTS) != 0)
    {
        ERROR("boot", "PMM tests failed, unknown error.");
        pmm_error_count++;
    }

    INFO("boot", "Initialized PMM (%d errors reported), %d tests ran", pmm_error_count, _PMM_TESTS);
    if (pmm_error_count > 0)
    {
        ERROR("boot", "Errors during PMM initialization or tests. Halting.");
        hcf();
    }

    int vmm_error_count = 0;

    __kernel_phys_base = kernel_address_request.response->physical_base;
    __kernel_virt_base = kernel_address_request.response->virtual_base;

    if (vmm_init() != 0)
    {
        ERROR("boot", "Failed to initialize VMM, unknown error.");
        vmm_error_count++;
    }

    vma_context_t *ctx = vma_create_context(kernel_pagemap);
    if (ctx == NULL)
    {
        ERROR("boot", "Failed to create VMA context for kernel.");
        vmm_error_count++;
    }

    test_vmm(_VMM_TESTS, ctx);
    __kernel_vma_context = ctx;

    INFO("boot", "Initialized VMM (%d errors reported), %d tests ran", vmm_error_count, _VMM_TESTS);
    if (vmm_error_count > 0)
    {
        ERROR("boot", "Errors during VMM initialization or tests. Halting.");
        hcf();
    }

    // Filesystem initialization
    int vfs_error_count = 0;

    if (vfs_init() != 0)
    {
        ERROR("boot", "Failed to initialize VFS, unknown error.");
        vfs_error_count++;
    }

    usize ramfs_size = module_request.response->modules[0]->size;
    u8 *ramfs = (u8 *)module_request.response->modules[0]->address;
    if (ramfs_init(ramfs, ramfs_size) != 0)
    {
        ERROR("boot", "Failed to initialize RAMFS, unknown error.");
        vfs_error_count++;
    }

    INFO("boot", "Initialized VFS (%d errors reported)", vfs_error_count);
    if (vfs_error_count > 0)
    {
        ERROR("boot", "Errors during VFS initialization. Halting.");
        hcf();
    }

    INFO("boot", "Finished initializing Nekonix.");
    test();

    hlt();
}
