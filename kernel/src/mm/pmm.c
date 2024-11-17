#include <mm/pmm.h>

pmm_stack_t stack;
struct limine_memmap_response *_memmap;

int pmm_init(struct limine_memmap_response *memmap)
{
    u64 free_pages = 0;
    _memmap = memmap;

    DEBUG("mm", "----- PMM INFO -----");
    for (u64 i = 0; i < memmap->entry_count; i++)
    {
        if (memmap->entries[i]->type == LIMINE_MEMMAP_USABLE)
        {
            DEBUG("mm", " - USABLE ENTRY\t\t@ 0x%.16llx, size: 0x%.16llx", memmap->entries[i]->base, memmap->entries[i]->length);
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
            DEBUG("mm", " - STACK START\t\t@ 0x%.16llx", stack.pages);
            break;
        }
    }

    for (u64 i = 0; i < memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = memmap->entries[i];
        for (u64 j = 0; j < entry->length; j += PAGE_SIZE)
        {
            stack.pages[stack.idx++] = entry->base + j;
        }
    }

    stack.max = stack.idx;
    DEBUG("mm", " - MAX INDEX:\t\t%d", stack.max);
    DEBUG("mm", " - CURRENT INDEX:\t%d", stack.idx);
    DEBUG("mm", "--------------------");

    return 0;
}

void *pmm_request_page()
{
    if (stack.idx == 0)
    {
        ERROR("mm", "No more pages available.");
        return NULL;
    }

    u64 page_addr = stack.pages[--stack.idx];
    return (void *)page_addr;
}

void pmm_free_page(void *ptr)
{
    if (ptr == NULL)
    {
        DEBUG("mm", "Attempt to free a NULL pointer.");
        return;
    }

    if (stack.idx >= stack.max)
    {
        ERROR("mm", "Stack overflow attempt while freeing a page.");
        return;
    }

    stack.pages[stack.idx++] = (u64)ptr;
}

void pmm_dump()
{
    INFO("mm", "----- PMM DUMP -----");
    INFO("mm", " - Total pages in stack: %d", stack.max);
    INFO("mm", " - Current free pages: %d", stack.idx);

    if (stack.idx == 0)
    {
        INFO("mm", " - No free pages available.");
    }
    else
    {
        INFO("mm", " - First 10 free pages (or fewer):");
        for (u64 i = 0; i < (stack.idx < 10 ? stack.idx : 10); i++)
        {
            INFO("mm", "   * Page %llu: 0x%.16llx", i, stack.pages[i]);
        }
    }

    INFO("mm", " - High memory address: 0x%.16llx", stack.pages[(stack.idx ? stack.idx : 1) - 1]);
    INFO("mm", " - Stack base address: 0x%.16llx", (u64)stack.pages);
    INFO("mm", "----------------------");
}