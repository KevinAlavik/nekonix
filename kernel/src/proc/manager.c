#include <proc/manager.h>
#include <mm/liballoc/liballoc.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DEVICES 256

handle_t *devices[MAX_DEVICES];

void device_manager_init(void)
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        devices[i] = NULL;
    }
}

int device_register(handle_t *handle)
{
    if (handle == NULL || handle->id >= MAX_DEVICES)
    {
        ERROR("manager", "Invalid device handle or ID out of bounds");
        return -1;
    }

    if (devices[handle->id] != NULL)
    {
        ERROR("manager", "Device ID %d already registered", handle->id);
        return -1;
    }

    devices[handle->id] = handle;

    DEBUG("manager", "Registered device, device id: %d", handle->id);
    return 0;
}

handle_t *device_get(u32 id)
{
    if (id >= MAX_DEVICES)
    {
        ERROR("manager", "Device ID %d out of bounds", id);
        return NULL;
    }

    handle_t *device = devices[id];
    if (device == NULL)
    {
        ERROR("manager", "Device not found, device id: %d", id);
        return NULL;
    }

    DEBUG("manager", "Found device, device id: %d, address: 0x%.16llx", id, (u64)device);
    return device;
}

int device_unregister(u32 id)
{
    if (id >= MAX_DEVICES)
    {
        ERROR("manager", "Device ID %d out of bounds", id);
        return -1;
    }

    if (devices[id] == NULL)
    {
        ERROR("manager", "Device ID %d not registered", id);
        return -1;
    }

    devices[id] = NULL;

    DEBUG("manager", "Unregistered device, device id: %d", id);
    return 0;
}
