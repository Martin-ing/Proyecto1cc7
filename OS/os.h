#ifndef OS_H
#define OS_H

void enable_irq(void);
void disable_irq(void);

void PUT32(unsigned int addr, unsigned int value);
unsigned int GET32(unsigned int addr);
void schedule(void);
void yield(void);

#endif // OS_H