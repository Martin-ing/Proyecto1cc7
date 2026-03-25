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
static ProcessQueue ready_queue;
Process *CurrProcess;

void schedule(void)
{
    if (CurrProcess != NULL) {
        CurrProcess->state = PROCESS_READY;
        sched_enqueue(&ready_queue, CurrProcess);
    }

    CurrProcess = sched_dequeue(&ready_queue);

    if (CurrProcess != NULL) {
        CurrProcess->state = PROCESS_RUNNING;
    }
}

int main(void) {
    sched_queue_init(&ready_queue);

    process_init(&p1, 1, P1_BASE, P1_STACK + PROCESS_STACK_SIZE);
    process_init(&p2, 2, P2_BASE, P2_STACK + PROCESS_STACK_SIZE);

    sched_enqueue(&ready_queue, &p1);
    sched_enqueue(&ready_queue, &p2);

    CurrProcess = sched_dequeue(&ready_queue);
    CurrProcess->state = PROCESS_RUNNING;

    timer_init();
    enable_irq();

    jump_to_process(CurrProcess->pc, CurrProcess->sp);

    while (1) { }
    return 0;
}