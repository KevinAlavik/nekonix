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

#endif // _TYPES_H