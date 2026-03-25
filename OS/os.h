#ifndef OS_H
#define OS_H

// Interrupt control
void enable_irq(void);

// Low-level memory access functions (implemented in root.s)
void PUT32(unsigned int addr, unsigned int value);
unsigned int GET32(unsigned int addr);
void schedule(void);

#endif // OS_H