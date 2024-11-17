#include <mm/vmm.h>
#include <lib/stdio.h>
#include <lib/types.h>

u64 *top_level;

u64 virt_to_phys(u64 virt)
{
    u64 pml1_idx = (virt & (u64)0x1ff << 12) >> 12;
    u64 pml2_idx = (virt & (u64)0x1ff << 21) >> 21;
    u64 pml3_idx = (virt & (u64)0x1ff << 30) >> 30;
    u64 pml4_idx = (virt & (u64)0x1ff << 39) >> 39;

    INFO("vmm", "PML1 index: %llu", pml1_idx);
    INFO("vmm", "PML2 index: %llu", pml2_idx);
    INFO("vmm", "PML3 index: %llu", pml3_idx);
    INFO("vmm", "PML4 index: %llu", pml4_idx);

    u64 pml4_value = top_level[pml4_idx];
    if (pml4_value & 1)
    {
        INFO("vmm", "PML3 is present!");
    }
    else
    {
        INFO("vmm", "PML3 is not present.");
    }

    u64 *pml3_table = (u64 *)HIGHER_HALF(pml4_value & 0x000FFFFFFFFFF000);
    u64 pml2_value = pml3_table[pml3_idx];

    if (pml2_value & 1)
    {
        INFO("vmm", "PML2 is present!");
    }
    else
    {
        INFO("vmm", "PML2 is not present.");
    }

    u64 *pml2_table = (u64 *)HIGHER_HALF(pml2_value & 0x000FFFFFFFFFF000);
    u64 pml1_value = pml2_table[pml2_idx];

    if (pml1_value & 1)
    {
        INFO("vmm", "PML1 is present!");
    }
    else
    {
        INFO("vmm", "PML1 is not present.");
    }

    u64 *pml1_table = (u64 *)HIGHER_HALF(pml1_value & 0x000FFFFFFFFFF000);
    u64 phys_addr = pml1_table[pml1_idx];

    return phys_addr;
}

int vmm_init()
{
    __asm__ volatile("mov %%cr3, %0" : "=r"(top_level) : : "memory");
    top_level = HIGHER_HALF(top_level);
    virt_to_phys(0xffff800000000000);
    return 0;
}
