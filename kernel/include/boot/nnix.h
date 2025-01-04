#ifndef NNIX_H
#define NNIX_H

#include <lib/stdio.h>
#include <lib/types.h>
#include <flanterm/flanterm.h>

#ifndef VERSION_MAJOR
#define VERSION_MAJOR "0"
#endif // VERSION_MAJOR

#ifndef VERSION_MINOR
#define VERSION_MINOR "0"
#endif // VERSION_MINOR

#ifndef VERSION_PATCH
#define VERSION_PATCH "0"
#endif // VERSION_PATCH

#ifndef VERSION_NOTE
#define VERSION_NOTE "-unkown"
#endif // VERSION_NOTE

#define _LOG(scope, level, fmt, ...) \
    printf("[ %s()::%s ]: " fmt "\n", __func__, level, ##__VA_ARGS__);

#define INFO(scope, fmt, ...) _LOG(scope, "INFO", fmt, ##__VA_ARGS__)

#ifdef _DEBUG
#define DEBUG(scope, fmt, ...) _LOG(scope, "DEBUG", fmt, ##__VA_ARGS__)
#define LA_DEBUG(scope, fmt, ...) WARN(scope, "DEBUG" fmt, ##__VA_ARGS__)
#else // _DEBUG
#define DEBUG(scope, fmt, ...) (void)0
#define LA_DEBUG(scope, fmt, ...) (void)0
#endif // _DEBUG

#define NOTE(scope, fmt, ...) _LOG(scope, "NOTE", fmt, ##__VA_ARGS__)
#define WARN(scope, fmt, ...) _LOG(scope, "WARN", fmt, ##__VA_ARGS__)
#define LA_WARN(scope, fmt, ...) WARN(scope, "WARN" fmt, ##__VA_ARGS__)
#define ERROR(scope, fmt, ...) _LOG(scope, "ERROR", fmt, ##__VA_ARGS__)

#ifndef _LA_LOGS
#undef LA_WARN
#undef LA_DEBUG
#define LA_WARN(scope, fmt, ...) (void)0
#define LA_DEBUG(scope, fmt, ...) (void)0
#endif // _LA_LOGS

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

extern u64 hhdm_offset;
extern char __limine_requests_start[];
extern char __limine_requests_end[];
extern char __text_start[];
extern char __text_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __data_start[];
extern char __data_end[];
extern u64 __kernel_phys_base;
extern u64 __kernel_virt_base;
extern void *__kernel_vma_context;
extern struct flanterm_context *ft_ctx;
extern struct limine_framebuffer *__kernel_framebuffer;
extern int flanterm_putchar(char);
extern int serial_putchar(char);

#define LS(path) vfs_debug_ls(vfs_lookup(path))

#endif // NNIX_H
