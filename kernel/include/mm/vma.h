#ifndef VMA_H
#define VMA_H

#include <lib/types.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

typedef struct vma_region
{
    u64 start;
    u64 size;
    u64 flags;
    struct vma_region *next;
    struct vma_region *prev;
} vma_region_t;

typedef struct vma_context
{
    u64 *pagemap;
    vma_region_t *root;
} vma_context_t;

vma_context_t *vma_create_context(u64 *pagemap);
void vma_destroy_context(vma_context_t *ctx);
void *vma_alloc(vma_context_t *ctx, u64 size, u64 flags);
void vma_free(vma_context_t *ctx, void *ptr);

#endif // VMA_H
