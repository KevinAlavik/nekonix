#include <dev/vfs.h>
#include <dev/rtc.h>

struct vnode *g_vfs_root = NULL;

int vfs_init()
{
    g_vfs_root = (struct vnode *)kmalloc(sizeof(struct vnode));
    if (!g_vfs_root)
    {
        return 1;
    }

    g_vfs_root->parent = NULL;
    g_vfs_root->child = NULL;
    g_vfs_root->next = NULL;
    g_vfs_root->name = strdup("/");
    g_vfs_root->type = VNODE_DIRECTORY;
    g_vfs_root->permissions = VNODE_PERMS_OWNER_READ | VNODE_PERMS_OWNER_WRITE | VNODE_PERMS_OWNER_EXECUTE |
                              VNODE_PERMS_GROUP_READ | VNODE_PERMS_GROUP_EXECUTE |
                              VNODE_PERMS_OTHERS_READ | VNODE_PERMS_OTHERS_EXECUTE;
    g_vfs_root->size = 0;
    g_vfs_root->uid = 0;
    g_vfs_root->data = NULL;
    g_vfs_root->creation_time = get_rtc_timestamp();

    DEBUG("vfs", "VFS initialized with root vnode: %s", g_vfs_root->name);
    return 0;
}

int vfs_create(vnode_t *parent, const char *name, vnode_type_t type, vnode_perms_t perms, vnode_t **new_node)
{
    if (!parent || !name || !new_node)
    {
        return 1;
    }

    vnode_t *node = (vnode_t *)kmalloc(sizeof(vnode_t));
    if (!node)
    {
        return 1;
    }

    node->parent = parent;
    node->child = NULL;
    node->next = NULL;

    node->name = strdup(name);
    if (!node->name)
    {
        kfree(node);
        return 1;
    }

    node->type = type;
    node->permissions = perms;
    node->size = 0;
    node->uid = 0;
    node->data = NULL;
    node->creation_time = get_rtc_timestamp();

    if (!parent->child)
    {
        parent->child = node;
    }
    else
    {
        vnode_t *current = parent->child;
        while (current->next)
        {
            current = current->next;
        }
        current->next = node;
    }

    *new_node = node;
    DEBUG("vfs", "Created Vnode: %s of type %d under parent: %s", name, type, parent->name);
    return 0;
}

int vfs_remove(vnode_t *parent, vnode_t *node)
{
    if (!parent || !node)
    {
        return 1;
    }

    vnode_t *prev = NULL;
    vnode_t *curr = parent->child;

    while (curr)
    {
        if (curr == node)
        {
            if (prev)
            {
                prev->next = curr->next;
            }
            else
            {
                parent->child = curr->next;
            }

            kfree(curr->name);
            kfree(curr);
            DEBUG("vfs", "Removed Vnode: %s", node->name);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }

    return 1;
}

int vfs_chmod(vnode_t *node, vnode_perms_t permissions)
{
    if (!node)
    {
        return 1;
    }

    node->permissions = permissions;
    DEBUG("vfs", "Changed permissions of Vnode: %s to %d", node->name, permissions);
    return 0;
}

int vfs_chown(vnode_t *node, int uid)
{
    if (!node)
    {
        return 1;
    }

    node->uid = uid;
    DEBUG("vfs", "Changed owner of Vnode: %s to %d", node->name, uid);
    return 0;
}

int vfs_read(vnode_t *node, u32 offset, u32 size, u8 *buffer)
{
    if (!node || !buffer)
    {
        return 1;
    }

    if (offset + size > node->size)
    {
        size = node->size - offset;
    }

    memcpy(buffer, node->data + offset, size);
    return size;
}

int vfs_write(vnode_t *node, u32 offset, u32 size, const u8 *buffer)
{
    if (!node || !buffer)
    {
        return 1;
    }

    u32 new_size = offset + size;
    DEBUG("vfs", "Attempting to write to Vnode: %s, offset: %u, size: %u, new size: %u", node->name, offset, size, new_size);

    if (new_size > node->size)
    {
        u8 *new_data = krealloc(node->data, new_size);
        if (!new_data)
        {
            return 1;
        }
        node->data = new_data;
        node->size = new_size;
    }

    memcpy(node->data + offset, buffer, size);
    return size;
}

vnode_t *vfs_search_node(vnode_t *parent, const char *name)
{
    if (!parent || !name)
    {
        return NULL;
    }

    vnode_t *curr = parent->child;
    while (curr)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }

    return NULL;
}

vnode_t *vfs_traverse_path(const char *path)
{
    if (!g_vfs_root || !path)
    {
        return NULL;
    }

    if (strcmp(path, "/") == 0)
    {
        return g_vfs_root;
    }

    char *path_copy = strdup(path);
    if (!path_copy)
    {
        return NULL;
    }

    vnode_t *node = g_vfs_root;
    char *token = strtok(path_copy, "/");
    while (token)
    {
        node = vfs_search_node(node, token);
        if (!node)
        {
            kfree(path_copy);
            DEBUG("vfs", "Node not found while traversing path: %s", token);
            return NULL;
        }
        token = strtok(NULL, "/");
    }

    kfree(path_copy);
    DEBUG("vfs", "Found Vnode: %s", node->name);
    return node;
}

vnode_t *vfs_lookup(const char *path)
{
    return vfs_traverse_path(path);
}

vnode_t *vfs_lookup_parent(const char *path)
{
    if (!path || strcmp(path, "/") == 0)
    {
        return NULL;
    }

    char *path_copy = strdup(path);
    if (!path_copy)
    {
        return NULL;
    }

    char *last_slash = strrchr(path_copy, '/');
    if (last_slash == path_copy)
    {
        *last_slash = '\0';
        last_slash = strrchr(path_copy, '/');
    }

    if (last_slash)
    {
        *last_slash = '\0';
    }

    vnode_t *parent_node = vfs_lookup(path_copy);
    kfree(path_copy);
    return parent_node;
}

const char *vfs_get_path(struct vnode *node)
{
    if (!node)
        return NULL;

    usize path_length = strlen(node->name);
    struct vnode *current = node->parent;

    while (current)
    {
        path_length += strlen(current->name) + 1;
        current = current->parent;
    }

    char *path = (char *)kmalloc(path_length + 1);
    if (!path)
        return NULL;

    path[0] = '\0';
    current = node->parent;

    while (current)
    {
        snprintf(path + strlen(path), path_length - strlen(path) + 1, "%s", current->name);
        current = current->parent;
    }

    snprintf(path + strlen(path), path_length - strlen(path) + 1, "%s", node->name);
    return path;
}

#ifdef _DEBUG
void vfs_debug_ls(struct vnode *node)
{
    if (!node)
    {
        ERROR("vfs", "Node is NULL, cannot list files.");
        return;
    }

    INFO("vfs", "Listing contents of directory: %s", vfs_get_path(node));

    for (struct vnode *current = node->child; current; current = current->next)
    {
        char perms[11] = {
            (current->type == VNODE_DIRECTORY) ? 'd' : '-',
            (current->permissions & VNODE_PERMS_OWNER_READ) ? 'r' : '-',
            (current->permissions & VNODE_PERMS_OWNER_WRITE) ? 'w' : '-',
            (current->permissions & VNODE_PERMS_OWNER_EXECUTE) ? 'x' : '-',
            (current->permissions & VNODE_PERMS_GROUP_READ) ? 'r' : '-',
            (current->permissions & VNODE_PERMS_GROUP_WRITE) ? 'w' : '-',
            (current->permissions & VNODE_PERMS_GROUP_EXECUTE) ? 'x' : '-',
            (current->permissions & VNODE_PERMS_OTHERS_READ) ? 'r' : '-',
            (current->permissions & VNODE_PERMS_OTHERS_WRITE) ? 'w' : '-',
            (current->permissions & VNODE_PERMS_OTHERS_EXECUTE) ? 'x' : '-',
            '\0'};

        u64 timestamp = current->creation_time;
        char formatted_time[20];
        snprintf(formatted_time, sizeof(formatted_time), "%04llu-%02llu-%02llu %02llu:%02llu:%02llu",
                 (timestamp >> 40) & 0xFFFF,
                 (timestamp >> 32) & 0xFF,
                 (timestamp >> 24) & 0xFF,
                 (timestamp >> 16) & 0xFF,
                 (timestamp >> 8) & 0xFF,
                 timestamp & 0xFF);

        INFO("vfs", "%s %s %8llu %s %s",
             perms, current->uid == 0 ? "root" : "unkown", current->size, formatted_time, current->name);
    }
}

#else
void vfs_debug_ls(struct vnode *node)
{
    (void)node;
}
#endif

char *vfs_debug_read(struct vnode *node)
{
    if (node == NULL)
    {
        ERROR("vfs", "The node is NULL.");
        return NULL;
    }

    if (node->type != VNODE_FILE)
    {
        ERROR("vfs", "The node is not a file.");
        return NULL;
    }

    u8 *buffer = (u8 *)kmalloc(node->size);
    usize bytes_read = vfs_read(node, 0, node->size, buffer);

    if (bytes_read > 0)
    {
        return (char *)buffer;
    }
    else
    {
        ERROR("vfs", "Failed to read the file.");
    }

    return NULL;
}