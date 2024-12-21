#ifndef HVFS_H
#define HVFS_H

#include <dev/vfs.h>
#include <lib/types.h>

// Open mode flags.
#define READ_ONLY 0
#define WRITE_ONLY 1
#define READ_WRITE 2
#define CREATE 4

// Nekonix FILE descriptor format.
typedef struct FILE
{
    int fd;              // File descriptor, has to be unique for each file.
    int uid;             // User ID of who opened this file.
    struct vnode *vnode; // Virtual node for this file, can be NULL.
    int mode;            // Open mode, e.g., READ_ONLY, WRITE_ONLY, READ_WRITE.
} FILE;

// HVFS Functions.
int hvfs_init();

// File operations.
FILE *open(const char *path, int mode);
int read(FILE *file, void *buffer, usize size);
int write(FILE *file, const void *buffer, usize size);
int chmod(FILE *file, int permissions);
int chown(FILE *file, int uid);
int close(FILE *file);

#endif // HVFS_H
