#include <proc/scheduler.h>
#include <lib/string.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <proc/elf.h>

static scheduler_t scheduler;
static u64 pid_counter = 0;

static process_t *process_create(u64 entry, u64 *pagemap, const char *name)
{
    process_t *proc = (process_t *)HIGHER_HALF(pmm_request_page());
    if (!proc)
    {
        ERROR("scheduler", "Failed to allocate process structure");
        return NULL;
    }

    proc->pid = pid_counter++;
    proc->state = PROCESS_NEW;
    proc->ctx.rip = entry;
    proc->ctx.rsp = (u64)HIGHER_HALF(pmm_request_page()) + 4095;

    if (!proc->ctx.rsp)
    {
        ERROR("scheduler", "Failed to allocate stack for process %llu", proc->pid);
        pmm_free_page((void *)proc);
        return NULL;
    }

    proc->ctx.cs = 0x08;
    proc->ctx.ss = 0x10;
    proc->ctx.rflags = 0x202;
    proc->pagemap = pagemap;
    proc->name = name;

    DEBUG("scheduler", "Process %llu created with entry 0x%lx", proc->pid, entry);
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

u64 scheduler_create_process(void (*entry)(void), const char *name)
{
    if (scheduler.process_count >= MAX_PROCESSES)
    {
        return -1; // Max process limit reached
    }
    process_t *proc = process_create((u64)entry, vmm_new_pagemap(), name);
    scheduler.processes[scheduler.process_count++] = proc;
    return proc->pid;
}

u64 scheduler_create_elf_process(u8 *data, const char *name)
{
    if (scheduler.process_count >= MAX_PROCESSES)
    {
        return -1; // Max process limit reached
    }

    u64 *pagemap = vmm_new_pagemap();
    u64 entry = elf_load(data, pagemap);
    if (entry == 0)
    {
        ERROR("scheduler", "Failed to load ELF, dropping proc.");
        return 0;
    }

    process_t *proc = process_create(entry, pagemap, name);
    scheduler.processes[scheduler.process_count++] = proc;
    return proc->pid;
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
    INFO("scheduler", "Process %d (%s) terminated with exit code %d", current_process->pid, current_process->name, exit_code);

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
