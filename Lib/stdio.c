#include "stdio.h"
#include "os.h"
#include "string.h"
#include <stdarg.h>

void READ(char *type, void *returnv){
    char input[16];

    uart_gets_input(input, sizeof(input));

    if(*type == '%'){
        type++;
        if(*type == 'f'){
            *(float *) returnv = uart_atof(input);
        }
        if(*type == 'd'){
            *(int *) returnv = uart_atoi(input);
        }
    }
}

void PRINT(const char *s, ...){
    va_list args;
    va_start(args, s);

    while (*s) {
        if(*s == '%'){
            char buffer[16];
            s++;
            if (*s == 'f') {
                double v = va_arg(args, double);
                uart_ftoa((float)v, buffer);
            }
            if(*s == 'd'){
                int v = va_arg(args, int);
                uart_itoa(v,&buffer);
            }
            uart_puts(buffer);
            s++;
        }
        else{
            uart_putc(*s++);
        }
    }

    va_end(args);
}