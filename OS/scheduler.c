#include "scheduler.h"
void sched_queue_init(ProcessQueue *q)
{
    q->tail  = NULL;
    q->count = 0;
}

void sched_enqueue(ProcessQueue *q, Process *p)
{
    if (p == NULL) return;

    if (q->tail == NULL) {
        p->next = p;
        q->tail = p;
    } else {
        p->next        = q->tail->next;
        q->tail->next  = p;
        q->tail        = p;
    }

    q->count++;
}

Process *sched_dequeue(ProcessQueue *q)
{
    Process *head;

    if (q->tail == NULL) return NULL;

    head = q->tail->next;

    if (head == q->tail) {
        q->tail = NULL;
    } else {
        q->tail->next = head->next;
    }

    head->next = NULL;
    q->count--;
    return head;
}

Process *sched_peek(ProcessQueue *q)
{
    if (q->tail == NULL) return NULL;
    return q->tail->next;
}

void sched_rotate(ProcessQueue *q)
{
    if (q->tail == NULL || q->count < 2) return;
    q->tail = q->tail->next;
}

int sched_is_empty(const ProcessQueue *q)
{
    return (q->tail == NULL);
}