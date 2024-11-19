#include <boot/nnix.h>
#include <mm/vma.h>

static spinlock_t liballoc_lock_var = SPINLOCK_INIT;

int liballoc_lock()
{
    spinlock_acquire(&liballoc_lock_var);
    return 0;
}

int liballoc_unlock()
{
    spinlock_release(&liballoc_lock_var);
    return 0;
}

void *liballoc_alloc(int pages)
{
    return vma_alloc((vma_context_t *)__kernel_vma_context, pages, VMM_PRESENT | VMM_WRITABLE);
}

int liballoc_free(void *ptr, int pages)
{
    (void)pages;
    vma_free(__kernel_vma_context, ptr);
    return 0;
}