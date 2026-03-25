#include "stdio.h"

__attribute__((section(".p2_text")))
void p2_start(void) {
    int i = 97;

    while (1) {
        PRINT("----From P2: %d\n", i);
        i++;
        if(i == 123){
            i=97;
        }
    }
}