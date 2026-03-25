#include "stdio.h"

__attribute__((section(".p1_text")))
void p1_start(void) {
    int i = 0;

    while (1) {
        PRINT("----From P1: %d\n", i);
        i++;
        if(i == 10){
            i=0;
        }
    }
}