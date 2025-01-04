#include <fs/ramfs.h>
#include <dev/vfs.h>
#include <lib/stdlib.h>
#include <lib/string.h>
#include <lib/stdio.h>
#include <lib/types.h>
#include <dev/rtc.h>

#define USTAR_HEADER_SIZE 512
#define NAME_SIZE 100

typedef struct ustar_header
{
    char name[NAME_SIZE];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[NAME_SIZE];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
} ustar_header_t;

#define TSUID 04000
#define TSGID 02000
#define TSVTX 01000

#define TUREAD 00400
#define TUWRITE 00200
#define TUEXEC 00100
#define TGREAD 00040
#define TGWRITE 00020
#define TGEXEC 00010
#define TOREAD 00004
#define TOWRITE 00002
#define TOEXEC 00001

u8 *ramfs_data;
usize ramfs_size;

int ramfs_parse(struct vnode *parent)
{
    if (!ramfs_data || ramfs_size == 0)
    {
        ERROR("ramfs", "No data to parse for RAM filesystem.");
        return 1;
    }

    usize offset = 0;

    while (offset < ramfs_size)
    {
        ustar_header_t *header = (ustar_header_t *)(ramfs_data + offset);

        if (header->name[0] == '\0')
        {
            DEBUG("ramfs", "Reached end of RAM filesystem data.");
            break;
        }

        u32 file_size = strtol(header->size, NULL, 8);
        bool is_directory = header->typeflag == '5';
        int mode = strtol(header->mode, NULL, 8);

        char *name = header->name;
        if (strncmp(header->name, "./", 2) == 0)
        {
            name += 2;
        }

        if (strlen(name) == 0 || strcmp(name, ".") == 0)
        {
            DEBUG("ramfs", "Skipping entry with empty name or current directory '.'");
            offset += USTAR_HEADER_SIZE + ((file_size + 511) & ~511);
            continue;
        }

        char *token = strtok(name, "/");
        struct vnode *current_parent = parent;

        while (token != NULL)
        {
            struct vnode *subdir = vfs_search_node(current_parent, token);

            if (!subdir)
            {
                if (is_directory)
                {
                    u32 permissions = 0;
                    permissions |= (mode & TUREAD) ? VNODE_PERMS_OWNER_READ : 0;
                    permissions |= (mode & TUWRITE) ? VNODE_PERMS_OWNER_WRITE : 0;
                    permissions |= (mode & TUEXEC) ? VNODE_PERMS_OWNER_EXECUTE : 0;
                    permissions |= (mode & TGREAD) ? VNODE_PERMS_GROUP_READ : 0;
                    permissions |= (mode & TGWRITE) ? VNODE_PERMS_GROUP_WRITE : 0;
                    permissions |= (mode & TGEXEC) ? VNODE_PERMS_GROUP_EXECUTE : 0;
                    permissions |= (mode & TOREAD) ? VNODE_PERMS_OTHERS_READ : 0;
                    permissions |= (mode & TOWRITE) ? VNODE_PERMS_OTHERS_WRITE : 0;
                    permissions |= (mode & TOEXEC) ? VNODE_PERMS_OTHERS_EXECUTE : 0;

                    if (vfs_create(current_parent, token, VNODE_DIRECTORY, permissions,
                                   &subdir) != 0)
                    {
                        ERROR("ramfs", "Failed to create directory \"%s\" under \"%s\".", token, current_parent->name);
                        return 1;
                    }

                    subdir->creation_time = get_rtc_timestamp();
                }
                else
                {
                    break;
                }
            }
            else
            {
                DEBUG("ramfs", "Found existing directory \"%s\".", token);
            }

            current_parent = subdir;
            token = strtok(NULL, "/");
        }

        if (!is_directory)
        {
            char *file_name = strtok(token, "/");
            if (file_name && strlen(file_name) > 0)
            {
                struct vnode *new_file;
                DEBUG("ramfs", "Creating file \"%s\" in directory \"%s\".", file_name, current_parent->name);

                u32 permissions = 0;
                permissions |= (mode & TUREAD) ? VNODE_PERMS_OWNER_READ : 0;
                permissions |= (mode & TUWRITE) ? VNODE_PERMS_OWNER_WRITE : 0;
                permissions |= (mode & TUEXEC) ? VNODE_PERMS_OWNER_EXECUTE : 0;
                permissions |= (mode & TGREAD) ? VNODE_PERMS_GROUP_READ : 0;
                permissions |= (mode & TGWRITE) ? VNODE_PERMS_GROUP_WRITE : 0;
                permissions |= (mode & TGEXEC) ? VNODE_PERMS_GROUP_EXECUTE : 0;
                permissions |= (mode & TOREAD) ? VNODE_PERMS_OTHERS_READ : 0;
                permissions |= (mode & TOWRITE) ? VNODE_PERMS_OTHERS_WRITE : 0;
                permissions |= (mode & TOEXEC) ? VNODE_PERMS_OTHERS_EXECUTE : 0;

                if (vfs_create(current_parent, file_name, VNODE_FILE, permissions, &new_file) != 0)
                {
                    ERROR("ramfs", "Failed to create file \"%s\" in directory \"%s\".", file_name, current_parent->name);
                    return 1;
                }

                new_file->creation_time = get_rtc_timestamp();
                new_file->data = (u8 *)header + USTAR_HEADER_SIZE;
                new_file->size = file_size;
                DEBUG("ramfs", "Copied %u bytes to file \"%s\".", file_size, file_name);
                DEBUG("ramfs", "File \"%s\" has permissions: %o", file_name, permissions);
                DEBUG("ramfs", "File \"%s\" has creation time: %u", file_name, new_file->creation_time);
                DEBUG("ramfs", "File \"%s\" has size: %u", file_name, new_file->size);
            }
        }

        offset += USTAR_HEADER_SIZE + ((file_size + 511) & ~511);
    }

    return 0;
}

int ramfs_init(u8 *data, usize size)
{
    if (!data || size == 0)
    {
        ERROR("ramfs", "Invalid data or size for RAM filesystem.");
        return 1;
    }

    ramfs_data = data;
    ramfs_size = size;

    struct vnode *initrd = g_vfs_root;
    initrd->type = VNODE_DIRECTORY;
    initrd->size = size;

    initrd->permissions = VNODE_PERMS_OWNER_READ | VNODE_PERMS_OWNER_WRITE | VNODE_PERMS_OWNER_EXECUTE |
                          VNODE_PERMS_GROUP_READ | VNODE_PERMS_GROUP_EXECUTE |
                          VNODE_PERMS_OTHERS_READ | VNODE_PERMS_OTHERS_EXECUTE;

    initrd->child = NULL;
    initrd->next = NULL;

    initrd->creation_time = get_rtc_timestamp();

    DEBUG("ramfs", "RAM filesystem mounted at \"%s\" with size %zu bytes.", initrd->name, size);

    return ramfs_parse(initrd);
}
