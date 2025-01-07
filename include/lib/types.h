/*
 * Copyright (c) 2024 Kevin Alavik <kevin@alavik.se>
 *
 * Licensed under the Nekonix License-v1
 * See the LICENSE file for more details.
 *
 * You are allowed to use, modify, and distribute this software in both private and commercial environments,
 * as long as you retain the copyright notice and do not remove or alter any copyright notice or attribution.
 * This software is provided "as-is" without warranty of any kind.
 */

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Basic types.

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef size_t usize;
typedef uintptr_t uptr;

// Generic spinlock implementation using atomic operations.
typedef struct spinlock
{
    volatile int locked;
} spinlock_t;

#define SPINLOCK_INIT {.locked = 0}

static inline void spinlock_init(spinlock_t *lock)
{
    lock->locked = 0;
}

#define spinlock_acquire(lock)                               \
    do                                                       \
    {                                                        \
        while (__sync_lock_test_and_set(&(lock)->locked, 1)) \
        {                                                    \
            while ((lock)->locked)                           \
                ;                                            \
        }                                                    \
    } while (0)

#define spinlock_release(lock)                \
    do                                        \
    {                                         \
        __sync_lock_release(&(lock)->locked); \
    } while (0)

#endif // TYPES_H