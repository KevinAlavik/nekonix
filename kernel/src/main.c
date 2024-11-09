#include <boot/limine.h>
#include <dev/serial_util.h>
#include <core/cpu.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

void sys_entry(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED)
    {
        outstr(0xE9, "ERROR: Limine Base Revision is not supported.");
        hcf();
    }

    outstr(0xE9, "OK.\n");

    hlt();
}
