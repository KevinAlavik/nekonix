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

#include <mm/pmm.h>
#include <lib/string.h>
#define KLOG_MODULE "pmm"
#include <utils/log.h>

pmm_stack_t stack;
struct limine_memmap_response *_memmap;

int pmm_init(struct limine_memmap_response *memmap)
{
    u64 free_pages = 0;
    _memmap = memmap;

    DEBUG("----- PMM INFO -----");
    for (u64 i = 0; i < memmap->entry_count; i++)
    {
        if (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            DEBUG(" - USABLE ENTRY\t\t@ 0x%.16llx, size: 0x%.16llx", memmap->entries[i]->base, memmap->entries[i]->length);
            free_pages += DIV_ROUND_UP(memmap->entries[i]->length, PAGE_SIZE);
        }
    }

    u64 array_size = ALIGN_UP(free_pages * 8, PAGE_SIZE);

    for (u64 i = 0; i < memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->length >= array_size && entry->type == LIMINE_MEMMAP_USABLE)
        {
            stack.pages = (uptr *)HIGHER_HALF(entry->base);
            entry->length -= array_size;
            entry->base += array_size;
            DEBUG(" - STACK START\t\t@ 0x%.16llx", stack.pages);
            break;
        }
    }

    for (u64 i = 0; i < memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            for (u64 j = 0; j < entry->length; j += PAGE_SIZE)
            {
                stack.pages[stack.idx++] = entry->base + j;
            }
        }
    }

    stack.max = stack.idx;
    DEBUG(" - MAX INDEX:\t\t%d", stack.max);
    DEBUG(" - CURRENT INDEX:\t\t%d", stack.idx);
    DEBUG("--------------------");

    return 0;
}

void *pmm_request_page()
{
    if (stack.idx == 0)
    {
        ERROR("No more pages available.");
        return NULL;
    }

    u64 page_addr = stack.pages[--stack.idx];
    memset(HIGHER_HALF(page_addr), 0, PAGE_SIZE);
    return (void *)page_addr;
}

void pmm_free_page(void *ptr)
{
    if (ptr == NULL)
    {
        DEBUG("Attempt to free a NULL pointer.");
        return;
    }

    if (stack.idx >= stack.max)
    {
        ERROR("Stack overflow attempt while freeing a page.");
        return;
    }

    stack.pages[stack.idx++] = (u64)ptr;
}
