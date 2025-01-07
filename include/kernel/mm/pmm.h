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

#ifndef PMM_H
#define PMM_H

#include <lib/types.h>
#include <nnix.h>
#include <limine.h>

#define PAGE_SIZE 4096

typedef struct pmm_stack
{
    uptr *pages;
    u64 idx;
    u64 max;
} pmm_stack_t;

#define DIV_ROUND_UP(x, y) (((u64)(x) + ((u64)(y) - 1)) / (u64)(y))
#define ALIGN_UP(x, y) (DIV_ROUND_UP(x, y) * (u64)(y))
#define ALIGN_DOWN(x, y) (((u64)(x) / (u64)(y)) * (u64)(y))

#define HIGHER_HALF(ptr) ((void *)((u64)ptr) + hhdm_offset)
#define PHYSICAL(ptr) ((void *)((u64)ptr) - hhdm_offset)

int pmm_init(struct limine_memmap_response *memmap);
void *pmm_request_page();
void pmm_free_page(void *ptr);

#endif // PMM_H