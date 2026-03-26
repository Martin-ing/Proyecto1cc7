#include "stdio.h"
#include "alphanumeric.h"

__attribute__((section(".p2_text")))
void p2_start(void) {
    char c = 'a';

    while (1) {
        disable_irq();
        PRINT("----From P2: %c\n", c);
        enable_irq();
        c++;
        if(c == ('z'+1)){
            c = 'a';
        }
    }
}