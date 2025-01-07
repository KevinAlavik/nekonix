/*
 * Copyright (c) 2024 Kevin Alavik <kevin@alavik.se>
 *
 * Licensed under the Nekonix License-v1
 * See the LICENSE file for more details.
 *
 * You are allowed to use, modify, and distribute this software in both private and commercial environments,
 * as long as you retain the copyright notice and do not remove or alter any copyright notice or attribution.
 * This software is provided "as-is" without warranty of any kind.
 */

#include <limine.h>
#include <nnix.h>
#include <lib/string.h>
#include <sys/portio.h>
#include <utils/printf.h>
#include <sys/cpu.h>
#define KLOG_MODULE "boot"
#include <utils/log.h>
#include <utils/flanterm/flanterm.h>
#include <utils/flanterm/backends/fb.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <utils/panic.h>
#include <mm/pmm.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0};
__attribute__((section(".limine_requests"))) static volatile struct limine_executable_file_request kernel_file_request = {
    .id = LIMINE_EXECUTABLE_FILE_REQUEST,
    .revision = 0,
    .response = NULL};
__attribute__((used, section(".limine_requests"))) static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests"))) static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .response = 0,
};
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;
struct cmdline_arg kernel_args[10];

int kernel_arg_count = 0;
bool kernel_debug_enabled = false;
int kernel_log_level = 0;

u64 hhdm_offset = 0;

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

int mirror_putchar(char ch)
{
    serial_putchar(ch);
    if (ft_ctx)
        flanterm_putchar(ch);
    return ch;
}

void kmain(void)
{
    putchar_impl = serial_putchar;
    printf("\033c");

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

    if (kernel_file_request.response == NULL)
    {
        printf("ERROR: No kernel file found\n");
        hcf();
    }

    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    ft_ctx = flanterm_fb_init(
        NULL, NULL, framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift, framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0, 0);

    if (ft_ctx == NULL)
    {
        ERROR("Failed to initialize framebuffer, falling back to serial output");
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

    kernel_debug_enabled = cmdline_get_bool("debug", false);
    kernel_log_level = cmdline_get_int("loglevel", 0);

    if (cmdline_get_bool("mirror", false))
        putchar_impl = mirror_putchar;

    INFO("Nekonix v%s.%s.%s%s", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_EXTRA);

    for (int i = 0; i < kernel_arg_count; i++)
    {
        INFO("Kernel argument: %s=%s", kernel_args[i].name, kernel_args[i].value ? kernel_args[i].value : "null");
    }

    if (gdt_init() != 0)
    {
        PANIC("Failed to initialize GDT, unknown error");
        hcf();
    }
    INFO("GDT initialized with 0 errors reported.");

    cli();
    if (idt_init() != 0)
    {
        PANIC("Failed to initialize IDT, unknown error");
        hcf();
    }
    sti();
    INFO("IDT initialized with 0 errors reported.");

    if (memmap_request.response == NULL)
    {
        PANIC("No memory map found");
        hcf();
    }

    if (hhdm_request.response == NULL)
    {
        PANIC("No HHDM offset found");
        hcf();
    }

    hhdm_offset = hhdm_request.response->offset;

    if (pmm_init(memmap_request.response) != 0)
    {
        PANIC("Failed to initialize PMM, unknown error");
        hcf();
    }
    INFO("PMM initialized with 0 errors reported.");

    int *a = (int *)HIGHER_HALF(pmm_request_page());
    if (a == NULL)
    {
        PANIC("Failed to allocate page");
        hcf();
    }
    *a = 42;
    DEBUG("Allocated page at 0x%llx, value: %d", a, *a);
    pmm_free_page((void *)a);

    halt();
}
