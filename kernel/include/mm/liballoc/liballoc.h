// Copyright: Durand Miller <clutter@djm.co.za>
#ifndef LIBALLOC_H
#define LIBALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <boot/nnix.h>

#define PREFIX(func) k##func

#ifdef _DEBUG
void liballoc_dump();
#endif // _DEBUG

extern int liballoc_lock();
extern int liballoc_unlock();
extern void *liballoc_alloc(size_t);
extern int liballoc_free(void *, size_t);

extern void *PREFIX(malloc)(size_t);
extern void *PREFIX(realloc)(void *, size_t);
extern void *PREFIX(calloc)(size_t, size_t);
extern void PREFIX(free)(void *);

#endif // LIBALLOC_H