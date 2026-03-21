#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

typedef enum {
    PROCESS_RUNNING = 0,
    PROCESS_READY,
    PROCESS_WAITING,
    PROCESS_SLEEPING,
    PROCESS_TERMINATED
} ProcessState;

typedef struct {
    uint32_t pid;

    uint32_t r[13];

    uint32_t sp;
    uint32_t lr;
    uint32_t pc;
    uint32_t spsr;
    ProcessState state;
} PCB;

struct Process;

typedef void (*ProcessEntry)(void);
typedef void (*ProcessSleepFunc)(struct Process *process, uint32_t ticks);

typedef struct Process {
    PCB pcb;

    const char *name;

    uint32_t *stack_base;
    uint32_t  stack_size;

    ProcessEntry entry;

    ProcessSleepFunc sleep;
} Process;

void process_init(Process *process,
                  uint32_t pid,
                  const char *name,
                  ProcessEntry entry,
                  uint32_t *stack_base,
                  uint32_t stack_size);

void process_sleep(Process *process, uint32_t ticks);

#endif