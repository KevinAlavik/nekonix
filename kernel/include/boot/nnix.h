#ifndef NNIX_H
#define NNIX_H

#include <lib/stdio.h>
#include <lib/types.h>

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

#ifndef _GRAPHICAL_LOG
#define _GRAPHICAL_LOG 0
#endif // _GRAPHICAL_LOG

#ifndef _MIRROR_LOG
#define _MIRROR_LOG 0
#endif // MIRROR_LOG

#ifndef _ERROR_LOG
#define _ERROR_LOG 0
#endif // __ERROR_LOG

#ifndef _NO_LOG
#define _NO_LOG 0
#endif // __NO_LOG

#ifndef _PMM_TESTS
#define _PMM_TESTS 10
#endif // _PMM_TESTS

#ifndef _VMM_TESTS
#define _VMM_TESTS 10
#endif // _VMM_TESTS

#if _NO_LOG
#define _LOG(scope, level, fmt, ...) (void)0
#else
#define _LOG(scope, level, fmt, ...) \
    printf("nnix@%s/%s: " fmt "\n", scope, level, ##__VA_ARGS__);
#endif // _NO_LOG

#if _ERROR_LOG
#define INFO(scope, fmt, ...) (void)0
#define DEBUG(scope, fmt, ...) (void)0
#define NOTE(scope, fmt, ...) (void)0
#define WARN(scope, fmt, ...) (void)0
#else // __ERROR_LOG
#define INFO(scope, fmt, ...) _LOG(scope, "info", fmt, ##__VA_ARGS__)
#ifdef _DEBUG
#define DEBUG(scope, fmt, ...) _LOG(scope, "debug", fmt, ##__VA_ARGS__)
#else // _DEBUG
#define DEBUG(scope, fmt, ...) (void)0
#endif // _DEBUG
#define NOTE(scope, fmt, ...) _LOG(scope, "note", fmt, ##__VA_ARGS__)
#define WARN(scope, fmt, ...) _LOG(scope, "warn", fmt, ##__VA_ARGS__)
#endif // _ERROR_LOG
#define ERROR(scope, fmt, ...) _LOG(scope, "error", fmt, ##__VA_ARGS__)

#ifdef _RELEASE
#undef WARN
#define WARN(scope, fmt, ...) (void)0
#endif // _RELEASE

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

#endif // NNIX_H