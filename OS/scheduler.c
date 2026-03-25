#include "scheduler.h"

/* ------------------------------------------------------------------ */
void sched_queue_init(ProcessQueue *q)
{
    q->tail  = NULL;
    q->count = 0;
}

/* ------------------------------------------------------------------ */
/*  Estructura interna tras enqueue(A), enqueue(B), enqueue(C):
 *
 *      tail
 *       ↓
 *      [C] → [A] → [B] → [C]   (circular)
 *              ↑ head
 *
 *  head = tail->next
 *  Para encolar D:
 *      D->next  = tail->next   (D apunta al head actual)
 *      tail->next = D          (el tail viejo apunta a D)
 *      tail = D                (D es el nuevo tail)
 * ------------------------------------------------------------------ */
void sched_enqueue(ProcessQueue *q, Process *p)
{
    if (p == NULL) return;

    if (q->tail == NULL) {
        /* Cola vacía: el proceso se apunta a sí mismo */
        p->next = p;
        q->tail = p;
    } else {
        /* Inserta entre tail y head */
        p->next        = q->tail->next;  /* p → head       */
        q->tail->next  = p;              /* tail → p       */
        q->tail        = p;              /* tail avanza a p */
    }

    q->count++;
}

/* ------------------------------------------------------------------ */
/*  Retira el head (tail->next) de la cola.
 *  No modifica p->next para no perder el puntero si se re-encola. */
Process *sched_dequeue(ProcessQueue *q)
{
    Process *head;

    if (q->tail == NULL) return NULL;   /* cola vacía */

    head = q->tail->next;               /* head = tail->next */

    if (head == q->tail) {
        /* Era el único elemento */
        q->tail = NULL;
    } else {
        q->tail->next = head->next;     /* tail salta al siguiente */
    }

    head->next = NULL;
    q->count--;
    return head;
}

/* ------------------------------------------------------------------ */
Process *sched_peek(ProcessQueue *q)
{
    if (q->tail == NULL) return NULL;
    return q->tail->next;               /* head sin dequeuar */
}

/* ------------------------------------------------------------------ */
/*  Round-robin: avanza tail una posición.
 *  Después de esto, el antiguo head se convierte en el nuevo tail
 *  y el antiguo segundo elemento pasa a ser el nuevo head.
 *
 *  Útil en el ISR del timer: llamas sched_rotate() y luego
 *  sched_peek() te da el próximo proceso a ejecutar.          */
void sched_rotate(ProcessQueue *q)
{
    if (q->tail == NULL || q->count < 2) return;
    q->tail = q->tail->next;            /* tail avanza → el viejo head */
}

/* ------------------------------------------------------------------ */
int sched_is_empty(const ProcessQueue *q)
{
    return (q->tail == NULL);
}