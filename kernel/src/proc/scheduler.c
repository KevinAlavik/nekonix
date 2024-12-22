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

    vma_free(proc_list[index]->vma_ctx, (void *)proc_list[index]->ctx.rsp);
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
    WARN("proc", "Process %d exited with code %llu", proc->pid, exit_code);
}

void proc_watchdog()
{
    DEBUG("proc", "Spawning process ");
    proc_t *proc = current_proc;
    if (proc->exited)
    {
        WARN("proc", "Process %llu exited with code %llu", proc->pid, proc->exit_code);
        proc_remove(proc->pid);
    }

    vmm_switch_pagemap(proc->pagemap);
    proc->entry();

    while (1)
    {
        // Keeps the process running. Can be modified for more complex tasks.
    }
}

int scheduler_init()
{
    proc_count = 0;
    current_proc_idx = 0;
    return 0;
}

void proc_spawn(void (*entry)(void))
{
    DEBUG("proc", "Spawning process at 0x%.16llx", (u64)entry);
    if (proc_count >= MAX_PROCESSES)
    {
        return;
    }

    proc_t *proc = (proc_t *)HIGHER_HALF(pmm_request_page());
    proc->pid = pid++;
    proc->ctx.rip = (u64)proc_watchdog;
    proc->ctx.rsp = (u64)HIGHER_HALF(pmm_request_page()) + 4095;
    proc->ctx.cs = 0x08;
    proc->ctx.ss = 0x10;
    proc->ctx.rflags = 0x202;
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
        return; // No processes to schedule
    }

    proc_t *proc = proc_list[current_proc_idx];
    if (proc == NULL)
    {
        return; // Invalid process
    }

    // Save the context of the current process
    if (current_proc != NULL)
    {
        // memcpy(&current_proc->ctx, frame, sizeof(int_frame_t));
    }

    // If the current process has exited, remove it and switch to the next one
    if (current_proc != NULL && current_proc->exited)
    {
        DEBUG("proc", "Process %d has exited. Removing...", current_proc->pid);
        proc_remove(current_proc_idx);
        if (proc_count == 0) // No processes left
        {
            return;
        }
        proc = proc_list[current_proc_idx];
    }

    // Switch to the next process
    current_proc = proc;

    // If the new process has no pagemap, create a new one
    if (current_proc->pagemap == NULL)
    {
        DEBUG("proc", "Creating new pagemap for process %d", current_proc->pid);
        current_proc->pagemap = vmm_new_pagemap();
    }

    // Switch the page map to the current process's pagemap
    DEBUG("proc", "Switching to pagemap 0x%.16llx for process %d", (u64)current_proc->pagemap, current_proc->pid);
    vmm_switch_pagemap(current_proc->pagemap);

    // Copy the saved context to the frame
    memcpy(frame, &current_proc->ctx, sizeof(int_frame_t));

    // Update the index to the next process in the list
    current_proc_idx = (current_proc_idx + 1) % proc_count;
    DEBUG("proc", "Switched to process %d", current_proc->pid);
}

proc_t *scheduler_get_current_proc()
{
    return current_proc;
}
