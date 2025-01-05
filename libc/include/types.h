#ifndef _TYPES_H
#define _TYPES_H

#ifndef __SIZE_TYPE__
#define __SIZE_TYPE__ long unsigned int
#endif

#ifdef NULL
#undef NULL
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef __cplusplus
#ifndef __bool_true_false_are_defined
#else
#undef bool
#undef true
#undef false
#endif

#define bool _Bool
#define true 1
#define false 0
#endif

typedef signed char s8;
typedef short s16;
typedef int s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#ifdef __x86_64__
typedef unsigned long long uptr;
typedef long long imax;
typedef unsigned long long umax;
#else
typedef unsigned int uptr;
typedef int imax;
typedef unsigned int umax;
#endif

#if _TYPES_BIT
typedef bool bit;
#endif
typedef u8 byte;
typedef u16 word;
typedef u32 dword;
typedef u64 qword;

typedef float _Complex f32_complex;
typedef double _Complex f64_complex;

typedef float f32;
typedef double f64;

typedef __SIZE_TYPE__ usize;

#define DEVICE_READY 0
#define DEVICE_BUSY 1

typedef struct
{
    u32 id;
    u8 (*poll)(void);
    u64 (*read)(void *);
    void (*write)(void *data, usize size);
} handle_t;

#define KEYCODE_SPACE 0x39
#define KEYCODE_ENTER 0x1C
#define KEYCODE_ESC 0x01
#define KEYCODE_BACKSPACE 0x0E
#define KEYCODE_UP 0x48
#define KEYCODE_DOWN 0x50
#define KEYCODE_LEFT 0x4B
#define KEYCODE_RIGHT 0x4D

typedef struct
{
    const char *sym;
    u8 scancode;
    bool released;
} key_t;

#endif // _TYPES_H