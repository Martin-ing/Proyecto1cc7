#include "plataform.h"
#include "os.h"

#define WDT1_BASE   0x44E35000
#define WDT_WSPR    (WDT1_BASE + 0x48)
#define WDT_WWPS    (WDT1_BASE + 0x34)

void watchdog_disable(void) {
    #if PLATFORM_TARGET == 0
        PUT32(WDT_WSPR, 0xAAAA);
        while (GET32(WDT_WWPS) & (1 << 4));
        PUT32(WDT_WSPR, 0x5555);
        while (GET32(WDT_WWPS) & (1 << 4));
    #endif
}