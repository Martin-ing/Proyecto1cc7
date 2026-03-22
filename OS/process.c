#include "process.h"

void process_init(Process *p,
                  uint32_t pid,
                  uint32_t pc,
                  uint32_t sp)
{
    int i;

    p->pid = pid;
    p->pc = pc;
    p->sp = sp;
    p->lr = 0;
    p->spsr = 0;
    p->state = PROCESS_READY;

    for (i = 0; i < 13; i++) {
        p->r[i] = 0;
    }
}