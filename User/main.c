#define UART0_BASE2 0x101F1000

#define UART_DR2    0x00  // Data Register
#define UART_FR2     0x18  // Flag Register
#define UART_FR_TXFF2 0x20 // Transmit FIFO Full

volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE2;

void uart_putc2(char c) {
    // Wait until there is space in the FIFO
    while (UART0[UART_FR2 / 4] & UART_FR_TXFF2);
    UART0[UART_DR2 / 4] = c;
}

void uart_puts2(const char *s) {
    while (*s) {
        uart_putc2(*s++);
    }
}

void main() {
    uart_puts2("Hello World\n");
    while (1); // Infinite loop to prevent exit
}