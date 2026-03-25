#ifndef UART_H
#define UART_H

// UART low-level
void uart_putc(char c);
char uart_getc(void);

// UART string I/O
void os_write(const char *s);
void os_read(char *buffer, int max_length);

void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);

#endif // UART_H