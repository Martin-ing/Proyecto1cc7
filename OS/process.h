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
    uint32_t r[13];     // R0-R12
    uint32_t sp;        // R13
    uint32_t lr;        // R14
    uint32_t pc;        // R15
    uint32_t spsr;
    ProcessState state;
} Process;

void process_init(Process *p,
                  uint32_t pid,
                  uint32_t pc,
                  uint32_t sp);

#endif