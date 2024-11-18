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
void vmm_switch_pagemap(u64 *new_pagemap);
u64 *vmm_new_pagemap();
void vmm_map(u64 *pagemap, u64 virt, u64 phys, u64 flags);
u64 virt_to_phys(u64 *pagemap, u64 virt);

#endif // VMM_H