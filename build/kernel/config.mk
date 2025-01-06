# Kernel Toolchain prefix, e.g. x86_64-elf-gcc-
TOOLCHAIN ?=

# Kernel build tools
CC := $(TOOLCHAIN)gcc
AR := $(TOOLCHAIN)ar
LD := $(TOOLCHAIN)ld
NASM := nasm

# Kernel output paths
BIN_DIR := bin
OBJ_DIR := obj

# Kernel build flags
IFLAGS := \
    -I../include/ \
    -I../include/kernel

CFLAGS := \
	-g -O2 -pipe \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel \
    $(IFLAGS)

CPPFLAGS := \
    -isystem $(CONF_PREFIX)/freestnd-c-hdrs-0bsd \
    -DLIMINE_API_REVISION=2 \
    -MMD \
    -MP

LDFLAGS := \
    -Wl,-m,elf_x86_64 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -Wl,--gc-sections \
    -T $(KERNEL_CONF_PREFIX)/linker.ld

NASMFLAGS := \
    -Wall \
    -f elf64
