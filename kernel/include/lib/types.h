#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;

typedef size_t usize;

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
