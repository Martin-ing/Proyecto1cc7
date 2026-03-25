#include "os.h"
#include "plataform.h"
#include "process.h"
#include "scheduler.h"
#include "uart.h"
#include "timer.h"

#define PROCESS_STACK_SIZE  0x1000

extern void jump_to_process(uint32_t pc, uint32_t sp);

static Process      p1;
static Process      p2;
static ProcessQueue ready_queue;          /* ← cola global del scheduler */

int main(void) {
    /* 1. Inicializa la cola */
    sched_queue_init(&ready_queue);

    /* 2. Inicializa los procesos */
    process_init(&p1, 1, P1_BASE, P1_STACK + PROCESS_STACK_SIZE);
    process_init(&p2, 2, P2_BASE, P2_STACK + PROCESS_STACK_SIZE);

    /* 3. Encola los procesos listos */
    sched_enqueue(&ready_queue, &p1);
    sched_enqueue(&ready_queue, &p2);

    /* 4. Arranca el timer y habilita IRQs */
    timer_init();
    enable_irq();

    /* 5. Salta al primer proceso en la cola */
    Process *first = sched_peek(&ready_queue);
    first->state   = PROCESS_RUNNING;
    jump_to_process(first->pc, first->sp);

    while (1) { }
    return 0;
}