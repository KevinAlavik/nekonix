#include <mm/vma.h>
#include <lib/string.h>

vma_context_t *vma_create_context(u64 *pagemap)
{
    vma_context_t *ctx = (vma_context_t *)HIGHER_HALF(pmm_request_page());
    memset(ctx, 0, sizeof(vma_context_t));
    ctx->root = (vma_region_t *)HIGHER_HALF(pmm_request_page());
    memset(ctx->root, 0, sizeof(vma_region_t));
    ctx->pagemap = pagemap;
    ctx->root->start = 0;
    ctx->root->size = 1;
    return ctx;
}

void vma_destroy_context(vma_context_t *ctx)
{
    vma_region_t *region = ctx->root;
    while (region != NULL)
    {
        vma_region_t *next = region->next;
        pmm_free_page((void *)PHYSICAL(region));
        region = next;
    }
    pmm_free_page((void *)PHYSICAL(ctx));
}

void *vma_alloc(vma_context_t *ctx, u64 size, u64 flags)
{
    if (ctx == NULL || ctx->root == NULL || ctx->pagemap == NULL)
    {
        ERROR("vma", "Invalid context or root passed to vma_alloc");
        return NULL;
    }

    vma_region_t *region = ctx->root->next;
    vma_region_t *new_region;
    vma_region_t *last_region;

    if (region == NULL)
    {
        new_region = (vma_region_t *)HIGHER_HALF(pmm_request_page());
        if (new_region == NULL)
        {
            ERROR("vma", "Failed to allocate new VMA region");
            return NULL;
        }

        memset(new_region, 0, sizeof(vma_region_t));
        last_region = ctx->root;
        goto skip;
    }

    while (region != ctx->root)
    {
        if (region->start + (region->size * PAGE_SIZE) - region->next->start >= size)
        {
            new_region = (vma_region_t *)HIGHER_HALF(pmm_request_page());
            if (new_region == NULL)
            {
                ERROR("vma", "Failed to allocate new VMA region");
                return NULL;
            }

            memset(new_region, 0, sizeof(vma_region_t));
            new_region->size = size;
            new_region->flags = flags;
            new_region->start = region->start + (region->size * PAGE_SIZE);
            new_region->next = region->next;
            new_region->prev = region;
            region->next = new_region;

            for (u64 i = 0; i < ALIGN_UP(new_region->size, PAGE_SIZE) / PAGE_SIZE; i++)
            {
                u64 page = (u64)pmm_request_page();
                if (page == 0)
                {
                    ERROR("vma", "Failed to allocate physical memory for VMA region");
                    return NULL;
                }
                vmm_map(ctx->pagemap, new_region->start + (i * PAGE_SIZE), page, new_region->flags);
            }

            return (void *)new_region->start;
        }
        region = region->next;
    }

    new_region = (vma_region_t *)HIGHER_HALF(pmm_request_page());
    if (new_region == NULL)
    {
        ERROR("vma", "Failed to allocate new VMA region");
        return NULL;
    }

    memset(new_region, 0, sizeof(vma_region_t));

    last_region = ctx->root;
    while (last_region->next != NULL)
    {
        last_region = last_region->next;
    }

skip:
    new_region->start = last_region->start + (last_region->size * PAGE_SIZE);
    new_region->size = size;
    new_region->flags = flags;
    new_region->next = NULL;
    new_region->prev = last_region;
    last_region->next = new_region;

    for (u64 i = 0; i < ALIGN_UP(new_region->size, PAGE_SIZE) / PAGE_SIZE; i++)
    {
        u64 page = (u64)pmm_request_page();
        if (page == 0)
        {
            ERROR("vma", "Failed to allocate physical memory for VMA region");
            return NULL;
        }
        vmm_map(ctx->pagemap, new_region->start + (i * PAGE_SIZE), page, new_region->flags);
    }

    return (void *)new_region->start;
}

void vma_free(vma_context_t *ctx, void *ptr)
{
    if (ctx == NULL)
    {
        ERROR("vma", "Invalid context passed to vma_free");
        return;
    }

    vma_region_t *region = ctx->root;
    while (region != NULL)
    {
        if (region->start == (u64)ptr)
        {
            break;
        }
        region = region->next;
    }

    if (region == NULL)
    {
        ERROR("vma", "Unable to find region to free at address 0x%.16llx", (u64)ptr);
        return;
    }

    vma_region_t *prev = region->prev;
    vma_region_t *next = region->next;

    for (u64 i = 0; i < region->size; i++)
    {
        pmm_free_page((void *)(virt_to_phys(ctx->pagemap, region->start + (i * PAGE_SIZE))));
        vmm_unmap(ctx->pagemap, region->start + (i * PAGE_SIZE));
    }

    if (prev != NULL)
    {
        prev->next = next;
    }

    if (next != NULL)
    {
        next->prev = prev;
    }

    if (region == ctx->root)
    {
        ctx->root = next;
    }

    pmm_free_page((void *)PHYSICAL(region));
    return;
}