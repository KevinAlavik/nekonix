#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <mm/vmm.h>
#include <sys/idt.h>
#include <lib/types.h>
#include <boot/nnix.h>

#define MAX_PROCESSES 256

typedef enum
{
    PROCESS_NEW,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct
{
    u64 pid;               // Process ID
    process_state_t state; // Current state of the process
    u64 *pagemap;          // Process page map
    u64 priority;          // Dynamic priority (higher value = higher priority)
    u64 time_slice;        // Time slice (quantum) for round-robin scheduling
    u64 elapsed_time;      // Elapsed time the process has been running
    int_frame_t ctx;       // Process context (CPU state)
} process_t;

typedef struct
{
    process_t *processes[MAX_PROCESSES];
    u32 process_count; // Number of processes in the system
    u32 current_index; // Index of the currently running process
    u64 tick_count;    // Global tick counter for time slice management
} scheduler_t;

// Function prototypes
void scheduler_init();
u64 scheduler_create_process(void (*entry)(void));
void scheduler_terminate_current_process(u64 exit_code);
void scheduler_tick();
void scheduler_context_switch(int_frame_t *frame);
process_t *scheduler_get_current_process();
void scheduler_idle();

#endif // SCHEDULER_H