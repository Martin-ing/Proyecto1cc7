#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include <stddef.h>

/* -----------------------------------------------------------
 * Cola circular de procesos implementada como lista enlazada
 * circular con puntero al nodo "tail" (último encolado).
 *
 *   tail → [P_n] → [P_1] → [P_2] → ... → [P_n] → ...
 *                   ↑ head implícito = tail->next
 *
 * Ventajas frente a un arreglo circular:
 *   - sin índices ni módulos
 *   - enqueue/dequeue O(1)
 *   - el round-robin es simplemente avanzar tail
 * ----------------------------------------------------------- */

typedef struct {
    Process  *tail;    /* apunta al ÚLTIMO proceso encolado    */
    uint32_t  count;   /* procesos actualmente en la cola      */
} ProcessQueue;

/* Inicializa la cola vacía */
void     sched_queue_init   (ProcessQueue *q);

/* Agrega un proceso al final de la cola */
void     sched_enqueue      (ProcessQueue *q, Process *p);

/* Retira y retorna el proceso al frente (head) */
Process *sched_dequeue      (ProcessQueue *q);

/* Consulta el proceso al frente SIN retirarlo */
Process *sched_peek         (ProcessQueue *q);

/* Rota la cola: el head pasa al tail (avance round-robin) */
void     sched_rotate       (ProcessQueue *q);

/* Retorna 1 si la cola está vacía */
int      sched_is_empty     (const ProcessQueue *q);

#endif /* SCHEDULER_H */