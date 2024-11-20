#ifndef ATHIX_DEV_VFS_H
#define ATHIX_DEV_VFS_H

#include <lib/string.h>
#include <lib/stdio.h>
#include <boot/nnix.h>
#include <lib/types.h>

typedef enum
{
    VNODE_DIRECTORY,
    VNODE_FILE
} vnode_type_t;

typedef enum
{
    VNODE_PERMS_READ = 0x0001,
    VNODE_PERMS_WRITE = 0x0002,
    VNODE_PERMS_EXECUTE = 0x0004
} vnode_perms_t;

typedef struct vnode
{

    struct vnode *parent;
    struct vnode *next;
    struct vnode *child;

    vnode_type_t type;
    vnode_perms_t permissions;

    char *name;
    usize size;

    u64 creation_time;
    u64 modification_time;

    int uid;

    u8 *data;
} vnode_t;

extern struct vnode *g_vfs_root;

int vfs_init();
int vfs_create(vnode_t *parent, const char *name, vnode_type_t type, vnode_perms_t perms, vnode_t **new_node);
int vfs_remove(struct vnode *parent, struct vnode *node);
int vfs_chmod(struct vnode *node, vnode_perms_t permissions);
int vfs_chown(struct vnode *node, int uid);
int vfs_read(struct vnode *node, u32 offset, u32 size, u8 *buffer);
int vfs_write(struct vnode *node, u32 offset, u32 size, const u8 *buffer);
struct vnode *vfs_search_node(struct vnode *parent, const char *name);
struct vnode *vfs_lookup(const char *path);
vnode_t *vfs_lookup_parent(const char *path);
const char *vfs_get_path(struct vnode *node);

void vfs_debug_ls(struct vnode *node);
char *vfs_debug_read(struct vnode *node);

#endif