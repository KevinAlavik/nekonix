#include <boot/limine.h>
#include <dev/serial_util.h>
#include <core/cpu.h>
#include <lib/stdio.h>
#include <boot/nnix.h>

__attribute__((used, section(".limine_requests"))) static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests_start"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end"))) static volatile LIMINE_REQUESTS_END_MARKER;

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

    // TODO: Properly setup COMX devices.
    printf("NNix (Nikonix) v%s.%s.%s%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_NOTE);

    hlt();
}
