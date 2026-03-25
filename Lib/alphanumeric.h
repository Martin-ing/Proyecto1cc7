#ifndef ALPHANUMERIC_H
#define ALPHANUMERIC_H

void uart_putnum(unsigned int num);

void uart_itoa(int num, char *buffer);
void uart_ftoa(float num, char *buffer);

int uart_atoi(const char *s);
float uart_atof(const char *s);

#endif // ALPHANUMERIC_H