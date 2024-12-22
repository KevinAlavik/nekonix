#include <mm/vmm.h>
#include <lib/stdio.h>
#include <lib/types.h>
#include <lib/string.h>
#include <boot/nnix.h>

u64 *kernel_pagemap;

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
    u64 phys_addr = pml1_table[pml1_idx] & 0x000FFFFFFFFFF000;

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

bool pagemap_is_valid(u64 *pagemap)
{
    if (!pagemap)
    {
        return false;
    }

    const u64 page_size = 4096;
    const u64 present_mask = 0x1;
    const u64 write_mask = 0x2;
    const u64 address_mask = 0xFFFFFFFFFFFFF000;

    for (size_t i = 0; i < 512; i++)
    {
        u64 entry = pagemap[i];

        if ((entry & present_mask) == 0)
        {
            continue;
        }

        u64 physical_addr = entry & address_mask;

        if (physical_addr % page_size != 0)
        {
            return false;
        }

        if ((entry & write_mask) == 0)
        {
            return false;
        }
    }

    return true;
}

u64 *vmm_new_pagemap()
{
    u64 *pagemap = (u64 *)HIGHER_HALF(pmm_request_page());
    if (pagemap == NULL)
    {
        ERROR("vmm", "Failed to allocate page for new pagemap.");
        return NULL;
    }
    memset(pagemap, 0, PAGE_SIZE);
    for (u64 i = 256; i < 512; i++)
    {
        pagemap[i] = kernel_pagemap[i];
        // DEBUG("vmm", "Copied kernel pagemap entry %d to new pagemap. (kernel pagemap: 0x%.16llx)", i, (u64)kernel_pagemap);
    }

    DEBUG("vmm", "Created new pagemap at 0x%.16llx", (u64)pagemap);
    return pagemap;
}

void vmm_switch_pagemap(u64 *new_pagemap)
{
    if (!pagemap_is_valid(new_pagemap))
    {
        ERROR("vmm", "Invalid pagemap.");
        return;
    }

    __asm__ volatile("movq %0, %%cr3" ::"r"(PHYSICAL((u64)new_pagemap)));
}

void vmm_destroy_pagemap(u64 *pagemap)
{
    for (u64 i = 0; i < 512; i++)
    {
        if (pagemap[i] & 1)
        {
            u64 *pml3_table = (u64 *)HIGHER_HALF(pagemap[i] & 0x000FFFFFFFFFF000);
            for (u64 j = 0; j < 512; j++)
            {
                if (pml3_table[j] & 1)
                {
                    u64 *pml2_table = (u64 *)HIGHER_HALF(pml3_table[j] & 0x000FFFFFFFFFF000);
                    for (u64 k = 0; k < 512; k++)
                    {
                        if (pml2_table[k] & 1)
                        {
                            u64 *pml1_table = (u64 *)HIGHER_HALF(pml2_table[k] & 0x000FFFFFFFFFF000);
                            for (u64 l = 0; l < 512; l++)
                            {
                                if (pml1_table[l] & 1)
                                {
                                    pmm_free_page((void *)(pml1_table[l] & 0x000FFFFFFFFFF000));
                                }
                            }
                            pmm_free_page((void *)pml1_table);
                        }
                    }
                    pmm_free_page((void *)pml2_table);
                }
            }
            pmm_free_page((void *)pml3_table);
        }
    }
    pmm_free_page((void *)pagemap);
}

int vmm_init()
{
    kernel_pagemap = (u64 *)HIGHER_HALF(pmm_request_page());
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
    INFO("vmm", "Mapped Limine Requests region.");

    for (uptr text = ALIGN_DOWN(__text_start, PAGE_SIZE); text < ALIGN_UP(__text_end, PAGE_SIZE); text += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, text, text - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT);
    }
    INFO("vmm", "Mapped .text region.");

    for (uptr rodata = ALIGN_DOWN(__rodata_start, PAGE_SIZE); rodata < ALIGN_UP(__rodata_end, PAGE_SIZE); rodata += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, rodata, rodata - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT | VMM_NX);
    }
    INFO("vmm", "Mapped .rodata region.");

    for (uptr data = ALIGN_DOWN(__data_start, PAGE_SIZE); data < ALIGN_UP(__data_end, PAGE_SIZE); data += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, data, data - __kernel_virt_base + __kernel_phys_base, VMM_PRESENT | VMM_WRITABLE | VMM_NX);
    }
    INFO("vmm", "Mapped .data region.");

    for (uptr gb4 = 0; gb4 < 0x100000000; gb4 += PAGE_SIZE)
    {
        vmm_map(kernel_pagemap, (uptr)gb4, gb4, VMM_PRESENT | VMM_WRITABLE);
        vmm_map(kernel_pagemap, (uptr)HIGHER_HALF(gb4), gb4, VMM_PRESENT | VMM_WRITABLE);
    }
    INFO("vmm", "Mapped HHDM.");

    vmm_switch_pagemap(kernel_pagemap);

    return 0;
}
