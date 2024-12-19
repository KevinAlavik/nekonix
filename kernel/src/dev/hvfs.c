#include <dev/vfs.h>
#include <dev/hvfs.h>

int hvfs_init() {
    if (g_vfs_root == NULL) {
        ERROR("hvfs", "VFS Root is not initialized.");
        return 1;
    }
    return 0;
}

FILE* open(const char* path, int mode) {
    FILE* fd = (FILE*)kmalloc(sizeof(FILE));
    if (!fd) {
        ERROR("hvfs", "Failed to allocate memory for file descriptor.");
        return NULL;
    }

    fd->mode = mode;

    if (mode & CREATE) {
        ERROR("hvfs", "HVFS is not able to create files at this moment.");
        return NULL;
    } else {
        fd->vnode = vfs_lookup(path);
        if (fd->vnode == NULL) {
            ERROR("hvfs", "File \"%s\" does not exist.", path);
            kfree(fd);
            return NULL;
        }
    }

    fd->fd = 0;  // Placeholder for file descriptor number.
    fd->uid = 0; // Default to root user, for now.

    return fd;
}

bool permission_check(vnode_t* node, int uid, vnode_perms_t required_perms) {
    if (node == NULL) return false;

    // Owner permissions
    if (node->uid == uid) {
        if (node->permissions & required_perms) {
            return true;
        }
    } else {
        if ((node->permissions & (required_perms << 8))) {
            return true;
        }
        ERROR("hvfs", "HVFS Cant handle group permissions. You are not the owner of this file.");
        return false;
    }

    // Group permissions (for now, assume user belongs to group; extend as needed)
    if ((node->permissions & (required_perms << 4))) {
        return true;
    }

    // Others permissions
    if ((node->permissions & (required_perms << 8))) {
        return true;
    }

    return false;
}

int read(FILE* file, void* buffer, usize size) {
    if (file == NULL) {
        ERROR("hvfs", "Invalid file descriptor.");
        return -1;
    }

    if (file->mode != READ_ONLY && file->mode != READ_WRITE) {
        ERROR("hvfs", "File is not opened for reading.");
        return -1;
    }

    if (file->vnode->type != VNODE_FILE) {
        ERROR("hvfs", "The node is not a file.");
        return -1;
    }

    if (!permission_check(file->vnode, file->uid, VNODE_PERMS_OWNER_READ)) {
        ERROR("hvfs", "Insufficient permissions to read the file.");
        return -1;
    }

    u8* data = file->vnode->data;
    usize bytes_read = size > file->vnode->size ? file->vnode->size : size;
    memcpy(buffer, data, bytes_read);

    return bytes_read;
}

int write(FILE* file, const void* buffer, usize size) {
    if (file == NULL) {
        ERROR("hvfs", "Invalid file descriptor.");
        return -1;
    }

    if (file->mode != WRITE_ONLY && file->mode != READ_WRITE) {
        ERROR("hvfs", "File is not opened for writing.");
        return -1;
    }

    if (file->vnode->type != VNODE_FILE) {
        ERROR("hvfs", "The node is not a file.");
        return -1;
    }

    if (!permission_check(file->vnode, file->uid, VNODE_PERMS_OWNER_WRITE)) {
        ERROR("hvfs", "Insufficient permissions to write to the file.");
        return -1;
    }

    int result = vfs_write(file->vnode, 0, size, (const u8*)buffer); // Write at offset 0 for simplicity.
    if (result < 0) {
        ERROR("hvfs", "Failed to write to vnode.");
        return -1;
    }

    return result;
}

int chmod(FILE* file, int permissions) {
    if (file == NULL) {
        ERROR("hvfs", "Invalid file descriptor.");
        return -1;
    }

    if (file->vnode == NULL) {
        ERROR("hvfs", "Invalid vnode associated with file.");
        return -1;
    }

    int result = vfs_chmod(file->vnode, permissions);
    if (result != 0) {
        ERROR("hvfs", "Failed to change permissions for vnode.");
        return -1;
    }

    return 0;
}

int chown(FILE* file, int uid) {
    if (file == NULL) {
        ERROR("hvfs", "Invalid file descriptor.");
        return -1;
    }

    if (file->vnode == NULL) {
        ERROR("hvfs", "Invalid vnode associated with file.");
        return -1;
    }

    int result = vfs_chown(file->vnode, uid);
    if (result != 0) {
        ERROR("hvfs", "Failed to change owner for vnode.");
        return -1;
    }

    return 0;
}

int close(FILE* file) {
    if (file == NULL) {
        ERROR("hvfs", "Invalid file descriptor.");
        return -1;
    }

    kfree(file);
    return 0;
}
