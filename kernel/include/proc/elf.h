#ifndef ELF_H
#define ELF_H

#include <lib/types.h>

u64 elf_load(u8 *data, u64 *pagemap);

#endif // ELF_H