#ifndef LOCK_H
#define LOCK_H

typedef struct
{
    volatile int locked;
} spinlock_t;

#define SPINLOCK_INIT {.locked = 0}

[[maybe_unused]] static void spinlock_acquire(spinlock_t *lock)
{
    while (__sync_lock_test_and_set(&lock->locked, 1))
    {
    }
}

[[maybe_unused]] static void spinlock_release(spinlock_t *lock)
{
    __sync_lock_release(&lock->locked);
}

#endif // LOCK_H