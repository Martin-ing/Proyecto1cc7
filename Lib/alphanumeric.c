#include "uart.h"
#include "alphanumeric.h"

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