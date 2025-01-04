#ifndef MANAGER_H
#define MANAGER_H

#include <boot/nnix.h>

typedef struct
{
    u32 id;
    u8 (*poll)(void);
    u64 (*read)(void *);
    void (*write)(void *data, usize size);
} handle_t;

#define DEVICE_READY 0
#define DEVICE_BUSY 1

int device_register(handle_t *handle);
handle_t *device_get(u32 id);
int device_unregister(u32 id);

#endif // MANAGER_H