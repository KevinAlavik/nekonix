#include <proc/scheduler.h>
#include <mm/vmm.h>
#include <mm/vma.h>
#include <lib/string.h>
#include <boot/nnix.h>
#include <sys/cpu.h>

#define MAX_PROCESSES 256

u64 pid = 0;
proc_t *current_proc;
proc_t *proc_list[MAX_PROCESSES];
u32 proc_count = 0;
u32 current_proc_idx = 0;

void proc_remove(u32 index)
{
    if (index >= proc_count)
    {
        return;
    }

    vma_free(proc_list[index]->vma_ctx, (void *)proc_list[index]->ctx->rsp);
    vma_free(proc_list[index]->vma_ctx, (void *)proc_list[index]);

    for (u32 i = index; i < proc_count - 1; ++i)
    {
        proc_list[i] = proc_list[i + 1];
    }
    proc_count--;
}

void proc_exit(proc_t *proc, u64 exit_code)
{
    proc->exited = true;
    proc->exit_code = exit_code;
    DEBUG("proc", "Process %d exited with code %llu", proc->pid, exit_code);
}

void proc_watchdog()
{
    proc_t *proc = current_proc;
    vmm_switch_pagemap(proc->pagemap);
    proc->entry();

    while (1)
    {
        // Keeps the process running. Can be modified for more complex tasks.
    }
}

void proc_watchdog_handler()
{
    while (1)
    {
        DEBUG("proc", "Watchdog tick");
        if (proc_count == 1 && proc_list[0] == current_proc)
        {
            proc_exit(current_proc, 0);
            return;
        }

        for (u32 i = 0; i < proc_count; i++)
        {
            proc_t *proc = proc_list[i];
            if (proc->exited)
            {
                DEBUG("proc", "Process %llu exited with code %llu", proc->pid, proc->exit_code);
                proc_remove(i);
                i--;
            }
        }
        asm volatile("pause");
    }
}

void scheduler_initialize()
{
    proc_count = 0;
    current_proc_idx = 0;
    proc_spawn(proc_watchdog_handler);
}

void proc_spawn(void (*entry)(void))
{
    DEBUG("proc", "Spawning process at 0x%.16llx", (u64)entry);
    if (proc_count >= MAX_PROCESSES)
    {
        return;
    }

    proc_t *proc = (proc_t *)kmalloc(sizeof(proc_t));
    proc->pid = pid++;
    proc->ctx = (int_frame_t *)kmalloc(sizeof(int_frame_t));
    proc->ctx->rip = (u64)proc_watchdog;
    proc->ctx->rsp = (u64)HIGHER_HALF(pmm_request_page()) + 4095;
    proc->ctx->cs = 0x08;
    proc->ctx->ss = 0x10;
    proc->ctx->rflags = 0x202;
    proc->pagemap = vmm_new_pagemap();
    proc->vma_ctx = vma_create_context(proc->pagemap);
    if (proc->vma_ctx == NULL)
    {
        ERROR("proc", "Failed to create VMA context for process %d", proc->pid);
        return;
    }

    proc->entry = entry;

    proc_list[proc_count++] = proc;
    DEBUG("proc", "Spawned process %d at 0x%.16llx", proc->pid, (u64)entry);
}

void scheduler_tick(int_frame_t *frame)
{
    DEBUG("proc", "Scheduler tick");
    if (proc_count == 0)
    {
        return;
    }

    proc_t *proc = proc_list[current_proc_idx];
    if (proc == NULL)
    {
        return;
    }

    if (current_proc != NULL)
    {
        memcpy(&current_proc->ctx, frame, sizeof(int_frame_t));
    }

    current_proc = proc;
    if (current_proc->exited)
    {
        proc_remove(current_proc_idx);
        return;
    }

    if (current_proc->pagemap == NULL)
    {
        DEBUG("proc", "Creating new pagemap for process %d", current_proc->pid);
        current_proc->pagemap = vmm_new_pagemap();
    }

    memcpy(frame, &current_proc->ctx, sizeof(int_frame_t));
    DEBUG("proc", "Switching to process %d", current_proc->pid);
    DEBUG("proc", "Switching to pagemap 0x%.16llx", (u64)current_proc->pagemap);
    vmm_switch_pagemap(current_proc->pagemap);
    DEBUG("proc", "Switched to process %d", current_proc->pid);
    current_proc_idx = (current_proc_idx + 1) % proc_count;
}

proc_t *scheduler_get_current_proc()
{
    return current_proc;
}
