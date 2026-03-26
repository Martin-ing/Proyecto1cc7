#include "uart.h"
#include "plataform.h"
#include "os.h"
#include "timer.h"

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

void timer_init(void) {
#if PLATFORM_TARGET == 1
    PUT32(PLATFORM_TIMER_BASE + 0x08, 0);         // detener timer
    PUT32(PLATFORM_TIMER_BASE + 0x00, 1000000);   // load value
    PUT32(PLATFORM_TIMER_BASE + 0x0C, 1);         // limpiar interrupcion
    PUT32(PLATFORM_TIMER_BASE + 0x08, 0xC2);      // enable + periodic + irq
#else
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
#endif
}

void timer_irq_handler(void) {
#if PLATFORM_TARGET == 1
    PUT32(PLATFORM_TIMER_BASE + 0x0C, 1);   // limpiar interrupcion SP804
    // acknowledge VIC si tienes el intc configurado
    os_write("Tick\n");
#else
    // Clear timer overflow flag
    PUT32(TISR, 0x2);

    // Acknowledge interrupt controller
    PUT32(INTC_CONTROL, 0x1);

    // Debug output
    os_write("Tick\n");
#endif
}