#include <proc/scheduler.h>
#include <lib/string.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

static scheduler_t scheduler;
static u64 pid_counter = 0;

static process_t *process_create(void (*entry)(void))
{
    process_t *proc = (process_t *)HIGHER_HALF(pmm_request_page());
    proc->pid = pid_counter++;
    proc->state = PROCESS_NEW;
    proc->priority = 0;     // Default priority
    proc->time_slice = 100; // Default time slice (quantum)
    proc->elapsed_time = 0;
    proc->ctx.rip = (u64)entry;
    proc->ctx.rsp = (u64)HIGHER_HALF(pmm_request_page()) + 4095;
    proc->ctx.cs = 0x08;
    proc->ctx.ss = 0x10;
    proc->ctx.rflags = 0x202;
    proc->pagemap = vmm_new_pagemap();
    return proc;
}

static void process_destroy(process_t *proc)
{
    vmm_destroy_pagemap(proc->pagemap);
    pmm_free_page(proc->pagemap);
    pmm_free_page((void *)proc->ctx.rsp);
    pmm_free_page((void *)proc);
}

void scheduler_init()
{
    scheduler.process_count = 0;
    scheduler.current_index = 0;
    scheduler.tick_count = 0;
}

u64 scheduler_create_process(void (*entry)(void))
{
    if (scheduler.process_count >= MAX_PROCESSES)
    {
        return -1; // Max process limit reached
    }
    process_t *proc = process_create(entry);
    scheduler.processes[scheduler.process_count++] = proc;
    return proc->pid;
}

void scheduler_tick()
{
    scheduler.tick_count++;

    process_t *current_process = scheduler.processes[scheduler.current_index];

    if (current_process->state == PROCESS_RUNNING)
    {
        current_process->elapsed_time++;
    }

    if (current_process->elapsed_time >= current_process->time_slice)
    {
        current_process->state = PROCESS_READY;
        scheduler.current_index = (scheduler.current_index + 1) % scheduler.process_count;
        scheduler.processes[scheduler.current_index]->state = PROCESS_RUNNING;
        current_process->elapsed_time = 0;
    }
}

void scheduler_context_switch(int_frame_t *frame)
{
    if (scheduler.process_count == 0)
    {
        return;
    }

    process_t *current_process = scheduler.processes[scheduler.current_index];

    if (current_process && current_process->state == PROCESS_RUNNING)
    {
        memcpy(&current_process->ctx, frame, sizeof(int_frame_t));
    }

    scheduler.current_index = (scheduler.current_index + 1) % scheduler.process_count;

    process_t *next_process = scheduler.processes[scheduler.current_index];

    if (next_process)
    {
        next_process->state = PROCESS_RUNNING;
        memcpy(frame, &next_process->ctx, sizeof(int_frame_t));
        vmm_switch_pagemap(next_process->pagemap);
    }
}

void scheduler_terminate_current_process(u64 exit_code)
{
    process_t *current_process = scheduler.processes[scheduler.current_index];
    if (current_process->state != PROCESS_RUNNING)
    {
        return;
    }

    current_process->state = PROCESS_TERMINATED;
    (void)exit_code; // Unused for now
    // current_process->exit_code = exit_code;

    process_destroy(current_process);

    for (u32 i = scheduler.current_index; i < scheduler.process_count - 1; ++i)
    {
        scheduler.processes[i] = scheduler.processes[i + 1];
    }

    scheduler.process_count--;
    if (scheduler.process_count == 0)
    {
        scheduler_idle();
    }
    else
    {
        scheduler.current_index %= scheduler.process_count;
    }
}

process_t *scheduler_get_current_process()
{
    if (scheduler.process_count == 0)
        return NULL;
    return scheduler.processes[scheduler.current_index];
}

void scheduler_idle()
{
    while (1)
    {
        asm volatile("hlt");
    }
}
