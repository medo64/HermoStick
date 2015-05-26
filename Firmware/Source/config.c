#include <xc.h>
#include "config.h"


void init() {
    //Interrupts
    GIE = 0;

    //Oscillator
    SPLLEN = 0;   //4x PLL is disabled
    IRCF3  = 1;   //4 MHz HF
    IRCF2  = 1;
    IRCF1  = 0;
    IRCF0  = 1;
    SCS1   = 1;   //Internal oscillator block
    SCS0   = 0;

    //Watchdog (2s)
    asm("CLRWDT");
    WDTPS4 = 0;
    WDTPS3 = 1;
    WDTPS2 = 0;
    WDTPS1 = 1;
    WDTPS0 = 1;
}
