#include "os.h"
#include "uart.h"
#include "plataform.h"

// ============================================================================
// BeagleBone Black UART0
// ============================================================================
#if PLATFORM_TARGET == 1
    #define UART_DR      0x00  // Data Register
    #define UART_FR      0x18  // Flag Register
    #define UART_FR_TXFF 0x20  // Transmit FIFO Full
    #define UART_FR_RXFE 0x10  // Receive FIFO Empty

    volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;
#else
    #define UART_THR       (UART0_BASE + 0x00)  // Transmit Holding Register
    #define UART_LSR       (UART0_BASE + 0x14)  // Line Status Register
    #define UART_LSR_THRE  0x20                 // Transmit Holding Register Empty
    #define UART_LSR_RXFE  0x10                 // Receive FIFO Empty
#endif

// ============================================================================
// UART low-level
// ============================================================================

// Send one character
void uart_putc(char c) {
#if PLATFORM_TARGET == 1
    while (UART0[UART_FR / 4] & UART_FR_TXFF);
    UART0[UART_DR / 4] = c;
#else
    while ((GET32(UART_LSR) & UART_LSR_THRE) == 0);
    PUT32(UART_THR, c);
#endif
}

// Receive one character
char uart_getc(void) {
#if PLATFORM_TARGET == 1
    while (UART0[UART_FR / 4] & UART_FR_RXFE);
    return (char)(UART0[UART_DR / 4] & 0xFF);
#else
    while ((GET32(UART_LSR) & UART_LSR_RXFE) != 0);
    return (char)(GET32(UART_THR) & 0xFF);
#endif
}

// ============================================================================
// UART string I/O
// ============================================================================

// Send a string
void os_write(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

// Optional alias if you want the other name too
void uart_puts(const char *s) {
    os_write(s);
}

// Read a line from UART
void os_read(char *buffer, int max_length) {
    int i = 0;
    char c;

    while (i < max_length - 1) {
        c = uart_getc();

        if (c == '\n' || c == '\r') {
            uart_putc('\n');
            break;
        }

        uart_putc(c);  // echo
        buffer[i++] = c;
    }

    buffer[i] = '\0';
}

// Optional alias if you want the other name too
void uart_gets_input(char *buffer, int max_length) {
    os_read(buffer, max_length);
}