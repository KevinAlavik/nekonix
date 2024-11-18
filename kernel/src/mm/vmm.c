#include <mm/vmm.h>
#include <lib/stdio.h>
#include <lib/types.h>
#include <lib/string.h>

u64 virt_to_phys(u64 *pagemap, u64 virt)
{
    u64 pml1_idx = (virt & (u64)0x1ff << 12) >> 12;
    u64 pml2_idx = (virt & (u64)0x1ff << 21) >> 21;
    u64 pml3_idx = (virt & (u64)0x1ff << 30) >> 30;
    u64 pml4_idx = (virt & (u64)0x1ff << 39) >> 39;

    if (!(pagemap[pml4_idx] & 1))
    {
        return 0;
    }

    u64 *pml3_table = (u64 *)HIGHER_HALF(pagemap[pml4_idx] & 0x000FFFFFFFFFF000);
    if (!(pml3_table[pml3_idx] & 1))
    {
        return 0;
    }

    u64 *pml2_table = (u64 *)HIGHER_HALF(pml3_table[pml3_idx] & 0x000FFFFFFFFFF000);
    if (!(pml2_table[pml2_idx] & 1))
    {
        return 0;
    }

    u64 *pml1_table = (u64 *)HIGHER_HALF(pml2_table[pml2_idx] & 0x000FFFFFFFFFF000);
    u64 phys_addr = pml1_table[pml1_idx];

    return phys_addr;
}

void vmm_map(u64 *pagemap, u64 virt, u64 phys, u64 flags)
{
    u64 pml1_idx = (virt & (u64)0x1ff << 12) >> 12;
    u64 pml2_idx = (virt & (u64)0x1ff << 21) >> 21;
    u64 pml3_idx = (virt & (u64)0x1ff << 30) >> 30;
    u64 pml4_idx = (virt & (u64)0x1ff << 39) >> 39;

    if (!(pagemap[pml4_idx] & 1))
    {
        pagemap[pml4_idx] = (u64)pmm_request_page() | flags;
    }

    u64 *pml3_table = (u64 *)HIGHER_HALF(pagemap[pml4_idx] & 0x000FFFFFFFFFF000);
    if (!(pml3_table[pml3_idx] & 1))
    {
        pml3_table[pml3_idx] = (u64)pmm_request_page() | flags;
    }

    u64 *pml2_table = (u64 *)HIGHER_HALF(pml3_table[pml3_idx] & 0x000FFFFFFFFFF000);
    if (!(pml2_table[pml2_idx] & 1))
    {
        pml2_table[pml2_idx] = (u64)pmm_request_page() | flags;
    }

    u64 *pml1_table = (u64 *)HIGHER_HALF(pml2_table[pml2_idx] & 0x000FFFFFFFFFF000);
    pml1_table[pml1_idx] = phys | flags;
}

void vmm_unmap(u64 *pagemap, u64 virt)
{
    u64 pml1_idx = (virt & (u64)0x1ff << 12) >> 12;
    u64 pml2_idx = (virt & (u64)0x1ff << 21) >> 21;
    u64 pml3_idx = (virt & (u64)0x1ff << 30) >> 30;
    u64 pml4_idx = (virt & (u64)0x1ff << 39) >> 39;

    if (!(pagemap[pml4_idx] & 1))
    {
        pagemap[pml4_idx] = 0;
        return;
    }

    u64 *pml3_table = (u64 *)HIGHER_HALF(pagemap[pml4_idx] & 0x000FFFFFFFFFF000);
    if (!(pml3_table[pml3_idx] & 1))
    {
        pml3_table[pml3_idx] = 0;
        return;
    }

    u64 *pml2_table = (u64 *)HIGHER_HALF(pml3_table[pml3_idx] & 0x000FFFFFFFFFF000);
    if (!(pml2_table[pml2_idx] & 1))
    {
        pml2_table[pml2_idx] = 0;
        return;
    }

    u64 *pml1_table = (u64 *)HIGHER_HALF(pml2_table[pml2_idx] & 0x000FFFFFFFFFF000);
    pml1_table[pml1_idx] = 0;
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}

u64 *vmm_new_pagemap(u64 *kernel_pagemap)
{
    u64 *pagemap = (u64 *)HIGHER_HALF(pmm_request_page());
    for (u64 i = 256; i < 512; i++)
    {
        pagemap[i] = kernel_pagemap[i];
    }

    return pagemap;
}

void vmm_switch_pagemap(u64 *new_pagemap)
{
    __asm__ volatile("movq %0, %%cr3" ::"r"(PHYSICAL((u64)new_pagemap)));
}

int vmm_init()
{
    u64 *kernel_pagemap = (u64 *)HIGHER_HALF(pmm_request_page());
    if (kernel_pagemap == NULL)
    {
        ERROR("vmm", "Failed to allocate page for kernel pagemap.");
        return 1;
    }

    memset(kernel_pagemap, 0, PAGE_SIZE);

    for (uptr reqs = ALIGN_DOWN(__limine_requests_start, PAGE_SIZE); reqs < ALIGN_UP(__limine_requests_end, PAGE_SIZE); reqs += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, reqs, reqs - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT | VMM_WRITABLE);
    }

    for (uptr text = ALIGN_DOWN(__text_start, PAGE_SIZE); text < ALIGN_UP(__text_end, PAGE_SIZE); text += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, text, text - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT);
    }

    for (uptr rodata = ALIGN_DOWN(__rodata_start, PAGE_SIZE); rodata < ALIGN_UP(__rodata_end, PAGE_SIZE); rodata += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, rodata, rodata - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT | VMM_NX);
    }

    for (uptr data = ALIGN_DOWN(__data_start, PAGE_SIZE); data < ALIGN_UP(__data_end, PAGE_SIZE); data += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, data, data - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT | VMM_WRITABLE | VMM_NX);
    }

    for (uptr gb4 = 0; gb4 < 0x100000000; gb4 += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, (uptr)gb4, gb4, VMM_PRESENT | VMM_WRITABLE);
        vmm_map(kernel_pagemap, (uptr)HIGHER_HALF(gb4), gb4, VMM_PRESENT | VMM_WRITABLE);
    }

    vmm_switch_pagemap(kernel_pagemap);

    return 0;
}