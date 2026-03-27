#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include <stddef.h>

typedef struct {
    Process  *tail;
    uint32_t  count;
} ProcessQueue;

void     sched_queue_init   (ProcessQueue *q);
void     sched_enqueue      (ProcessQueue *q, Process *p);
Process *sched_dequeue      (ProcessQueue *q);
Process *sched_peek         (ProcessQueue *q);
void     sched_rotate       (ProcessQueue *q);
int      sched_is_empty     (const ProcessQueue *q);

#endif /* SCHEDULER_H */