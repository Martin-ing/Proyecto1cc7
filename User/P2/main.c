#include "stdio.h"
#include "alphanumeric.h"

__attribute__((section(".p2_text")))
void p2_start(void) {
    char c = 'a';

    while (1) {
        PRINT("----From P2: %c\n", c);
        c++;
        if(c == ('z'+1)){
            c = 'a';
        }
    }
}