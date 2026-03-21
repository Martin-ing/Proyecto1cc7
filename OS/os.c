#include "os.h"
#include "plataform.h"

// ============================================================================
// BeagleBone Black UART0
// ============================================================================

#define UART_THR       (UART0_BASE + 0x00)  // Transmit Holding Register
#define UART_LSR       (UART0_BASE + 0x14)  // Line Status Register
#define UART_LSR_THRE  0x20                 // Transmit Holding Register Empty
#define UART_LSR_RXFE  0x10                 // Receive FIFO Empty

// ============================================================================
// BeagleBone Black DMTIMER2
// ============================================================================

#define TCLR                (DMTIMER2_BASE + 0x38)  // Timer Control Register
#define TCRR                (DMTIMER2_BASE + 0x3C)  // Timer Counter Register
#define TISR                (DMTIMER2_BASE + 0x28)  // Timer Interrupt Status Register
#define TIER                (DMTIMER2_BASE + 0x2C)  // Timer Interrupt Enable Register
#define TLDR                (DMTIMER2_BASE + 0x40)  // Timer Load Register

// ============================================================================
// BeagleBone Black Interrupt Controller (INTCPS)
// ============================================================================

#define INTC_MIR_CLEAR2     (INTCPS_BASE + 0xC8)    // Interrupt Mask Clear Register 2
#define INTC_CONTROL        (INTCPS_BASE + 0x48)    // Interrupt Controller Control
#define INTC_ILR68          (INTCPS_BASE + 0x110)   // Interrupt Line Register 68

// ============================================================================
// Clock Manager
// ============================================================================

#define CM_PER_TIMER2_CLKCTRL   (CM_PER_BASE + 0x80)  // Timer2 Clock Control

// ============================================================================
// UART low-level
// ============================================================================

// Send one character
void uart_putc(char c) {
    while ((GET32(UART_LSR) & UART_LSR_THRE) == 0);
    PUT32(UART_THR, c);
}

// Receive one character
char uart_getc(void) {
    while ((GET32(UART_LSR) & UART_LSR_RXFE) != 0);
    return (char)(GET32(UART_THR) & 0xFF);
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

// ============================================================================
// Number output helpers
// ============================================================================

// Print unsigned integer directly to UART
void uart_putnum(unsigned int num) {
    char buf[16];
    int i = 0;

    if (num == 0) {
        uart_putc('0');
        uart_putc('\n');
        return;
    }

    while (num > 0 && i < 15) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }

    uart_putc('\n');
}

// Convert int to string
void uart_itoa(int num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    int start = 0;
    int end = i - 1;
    char temp;

    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}
void uart_ftoa(float num, char *buffer) {
    int i = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    int intpart = (int)num;
    float decpart = num - (float)intpart;

    // Parte decimal convertida a entero "moviendo" el punto
    while (decpart - (float)((int)decpart)) {
        decpart = decpart * 10.0f;
    }

    int intdecpart = (int)decpart;

    if (intdecpart == 0) {
        buffer[i++] = '0';
    }

    while (intdecpart > 0 && i < 14) {
        buffer[i++] = '0' + (intdecpart % 10);
        intdecpart /= 10;
    }

    buffer[i++] = '.';

    if (intpart == 0) {
        buffer[i++] = '0';
    } else {
        while (intpart > 0 && i < 14) {
            buffer[i++] = '0' + (intpart % 10);
            intpart /= 10;
        }
    }

    if (is_negative) {
        buffer[i++] = '-';
    }

    buffer[i] = '\0';

    // reverse
    int start = 0;
    int end = i - 1;
    char temp;

    while (start < end) {
        temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

// ============================================================================
// String to number helpers
// ============================================================================

// String to int
int uart_atoi(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    while (s[i] >= '0' && s[i] <= '9') {
        num = num * 10 + (s[i] - '0');
        i++;
    }

    return sign * num;
}

// String to float
float uart_atof(const char *s) {
    float num = 0.0f;
    float dec = 0.0f;
    float decsize = 1.0f;
    float sign = 1.0f;
    int i = 0;
    int ondec = 0;

    if (s[i] == '-') {
        sign = -1.0f;
        i++;
    }

    while ((s[i] >= '0' && s[i] <= '9') || s[i] == '.') {
        if (s[i] == '.') {
            ondec = 1;
        } else {
            if (ondec) {
                dec = dec * 10.0f + (float)(s[i] - '0');
                decsize = decsize * 10.0f;
            } else {
                num = num * 10.0f + (float)(s[i] - '0');
            }
        }
        i++;
    }

    num = num + (dec / decsize);

    return sign * num;
}

// ============================================================================
// Timer
// ============================================================================

void timer_init(void) {
    // Enable timer2 clock
    PUT32(CM_PER_TIMER2_CLKCTRL, 0x2);

    // Unmask interrupt 68 and give it priority
    PUT32(INTC_MIR_CLEAR2, (1 << 4));
    PUT32(INTC_ILR68, 0x0);

    // Stop timer and clear pending flags
    PUT32(TCLR, 0x0);
    PUT32(TISR, 0x7);

    // Load start value for ~2 seconds at 24 MHz
    PUT32(TLDR, 0xFE91CA00);
    PUT32(TCRR, 0xFE91CA00);

    // Enable overflow interrupt and auto-reload
    PUT32(TIER, 0x2);
    PUT32(TCLR, 0x3);
}

void timer_irq_handler(void) {
    // Clear timer overflow flag
    PUT32(TISR, 0x2);

    // Acknowledge interrupt controller
    PUT32(INTC_CONTROL, 0x1);

    // Debug output
    os_write("Tick\n");
}

// ============================================================================
// Random
// ============================================================================

static unsigned int seed = 12345;

unsigned int rand(void) {
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}