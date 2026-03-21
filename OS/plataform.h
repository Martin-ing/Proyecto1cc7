#ifndef PLATFORM_H
#define PLATFORM_H

#ifndef PLATFORM_UART0_BASE
#error "PLATFORM_UART0_BASE no definido"
#endif

#ifndef PLATFORM_TIMER_BASE
#error "PLATFORM_TIMER_BASE no definido"
#endif

#ifndef PLATFORM_INTC_BASE
#error "PLATFORM_INTC_BASE no definido"
#endif

#define UART0_BASE       PLATFORM_UART0_BASE
#define DMTIMER2_BASE    PLATFORM_TIMER_BASE
#define INTCPS_BASE      PLATFORM_INTC_BASE
#define CM_PER_BASE      PLATFORM_CM_PER_BASE

#endif