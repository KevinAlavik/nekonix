#include <proc/elf.h>
#include <boot/nnix.h>
#include <sys/cpu.h>
#include <mm/vmm.h>
#include <mm/vma.h>
#include <lib/string.h>

#define ELF_MAGIC 0x464C457F

typedef struct
{
    u32 e_magic;
    u8 e_class;
    u8 e_data;
    u8 e_version;
    u8 e_osabi;
    u8 e_abiversion;
    u8 e_pad[7];
    u16 e_type;
    u16 e_machine;
    u32 e_version2;
    u64 e_entry;
    u64 e_phoff;
    u64 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
} __attribute__((packed)) elf_h_t;

typedef struct
{
    u32 p_type;
    u32 p_flags;
    u64 p_offset;
    u64 p_vaddr;
    u64 p_paddr;
    u64 p_filesz;
    u64 p_memsz;
    u64 p_align;
} __attribute__((packed)) elf_ph_t;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7

#define PF_X 0x1 // Execute
#define PF_W 0x2 // Write
#define PF_R 0x4 // Read

u64 elf_load(u8 *data, u64 *pagemap)
{
    elf_h_t *header = (elf_h_t *)data;

    if (header->e_magic != ELF_MAGIC)
    {
        ERROR("elf", "Invalid ELF magic: 0x%x", header->e_magic);
        return 0;
    }

    if (header->e_class != 2) // Ensure 64-bit ELF
    {
        ERROR("elf", "Unsupported ELF class: %u", header->e_class);
        return 0;
    }

    elf_ph_t *ph = (elf_ph_t *)(data + header->e_phoff);

    for (u16 i = 0; i < header->e_phnum; i++)
    {
        if (ph[i].p_type != PT_LOAD)
            continue;

        u64 vaddr_start = ALIGN_DOWN(ph[i].p_vaddr, PAGE_SIZE);
        u64 vaddr_end = ALIGN_UP(ph[i].p_vaddr + ph[i].p_memsz, PAGE_SIZE);
        u64 offset = ph[i].p_offset;

        u64 flags = VMM_PRESENT;
        if (ph[i].p_flags & PF_W)
            flags |= VMM_WRITABLE;
        if (!(ph[i].p_flags & PF_X))
            flags |= VMM_NX;

        for (u64 addr = vaddr_start; addr < vaddr_end; addr += PAGE_SIZE)
        {
            u64 phys = (u64)pmm_request_page();
            if (!phys)
            {
                ERROR("elf", "Out of physical memory");
                return 0;
            }

            vmm_map(pagemap, addr, phys, flags);

            u64 file_offset = offset + (addr - vaddr_start);
            if (file_offset < offset + ph[i].p_filesz)
            {
                u64 to_copy = PAGE_SIZE;
                if (file_offset + PAGE_SIZE > offset + ph[i].p_filesz)
                    to_copy = offset + ph[i].p_filesz - file_offset;

                memcpy((void *)HIGHER_HALF(phys), data + file_offset, to_copy);
            }
            else
            {
                memset((void *)HIGHER_HALF(phys), 0, PAGE_SIZE);
            }
        }
    }

    DEBUG("elf", "ELF loaded successfully, entry: 0x%lx", header->e_entry);
    return header->e_entry;
}
