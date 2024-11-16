#ifndef PMM_H
#define PMM_H

#define PAGE_SIZE 4096

#include <lib/types.h>
#include <boot/nnix.h>
#include <boot/limine.h>

typedef struct pmm_stack
{
    uptr *pages;
    u64 idx;
    u64 max;
} pmm_stack_t;

#define DIV_ROUND_UP(a, b) (((a) + (b) - 1) / (b))
#define ALIGN_UP(a, b) (((a) + (b) - 1) & ~((b) - 1))
#define HIGHER_HALF(a) ((u64)a + hhdm_offset)

int pmm_init(struct limine_memmap_response *memmap);
void *pmm_request_page();
void pmm_free_page(void *ptr);
void *pmm_request_pages(usize pages);
void pmm_free_pages(void *ptr, usize pages);
void pmm_dump();

#endif // PMM_H