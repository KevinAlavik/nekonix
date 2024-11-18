#ifndef VMM_H
#define VMM_H

#include <lib/types.h>
#include <boot/nnix.h>
#include <mm/pmm.h>

#define VMM_PRESENT 1
#define VMM_WRITABLE 2
#define VMM_NX (1ull << 63)
#define VMM_USER 4

int vmm_init();

#endif // VMM_H