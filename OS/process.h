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

    struct Process *next;
} Process;

void process_init(Process *p,
                  uint32_t pid,
                  uint32_t pc,
                  uint32_t sp);

#endif