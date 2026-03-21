#include "process.h"

void process_init(Process *process,
                  uint32_t pid,
                  const char *name,
                  ProcessEntry entry,
                  uint32_t *stack_base,
                  uint32_t stack_size)
{
    int i;

    process->name = name;
    process->stack_base = stack_base;
    process->stack_size = stack_size;
    process->entry = entry;

    process->sleep = process_sleep;

    process->pcb.pid = pid;
    process->pcb.state = PROCESS_READY;

    for (i = 0; i < 13; i++) {
        process->pcb.r[i] = 0;
    }
    process->pcb.sp = (uint32_t)(stack_base + (stack_size / sizeof(uint32_t)));

    process->pcb.lr = 0;
    process->pcb.pc = (uint32_t)entry;
    process->pcb.spsr = 0;
}

void process_sleep(Process *process, uint32_t ticks)
{
    (void)ticks;
    process->pcb.state = PROCESS_SLEEPING;
}