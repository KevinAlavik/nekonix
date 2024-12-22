#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <mm/vmm.h>
#include <mm/vma.h>
#include <sys/idt.h>
#include <boot/nnix.h>
#include <lib/types.h>

#define MAX_PROCESSES 256

typedef struct
{
    u64 pid;
    int_frame_t *ctx;
    void (*entry)(void);
    bool exited;
    u64 exit_code;
    u64 *pagemap;
    vma_context_t *vma_ctx;
} proc_t;

extern u64 pid;
extern proc_t *current_proc;
extern proc_t *proc_list[MAX_PROCESSES];
extern u32 proc_count;
extern u32 current_proc_idx;

void proc_remove(u32 index);
void proc_exit(proc_t *proc, u64 exit_code);
void proc_watchdog();
void proc_watchdog_handler();
int scheduler_init();
void proc_spawn(void (*entry)(void));
void scheduler_tick(int_frame_t *frame);
proc_t *scheduler_get_current_proc();

#endif // SCHEDULER_H
