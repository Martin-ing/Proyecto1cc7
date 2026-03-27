#include "stdio.h"
#include "os.h"

__attribute__((section(".p1_text")))
void p1_start(void) {
    int i = 0;

    while (1) {
        disable_irq();
        PRINT("----From P1: %d\n", i);
        enable_irq();
        i++;
        if(i == 10){
            i=0;
        }
    }
}