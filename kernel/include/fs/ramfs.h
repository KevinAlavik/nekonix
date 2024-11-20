#ifndef RAMFS_H
#define RAMFS_H

#include <dev/vfs.h>
#include <lib/types.h>

int ramfs_init(u8 *data, usize size);

#endif // RAMFS_H
