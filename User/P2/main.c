#include "stdio.h"
#include "alphanumeric.h"

__attribute__((section(".p2_text")))
void p2_start(void) {
    int i = 97;

    while (1) {
        char buffer[12];
        uart_itoa(i, buffer);
        PRINT("----From P2: %s\n", buffer);
        i++;
        if(i == 123){
            i=97;
        }
    }
}