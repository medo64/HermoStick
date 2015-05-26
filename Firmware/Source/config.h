#ifndef CONFIG_H
#define CONFIG_H

#include <xc.h>


#pragma config FOSC     = INTOSC
#pragma config WDTE     = ON
#pragma config PWRTE    = ON
#pragma config MCLRE    = OFF
#pragma config CP       = OFF
#pragma config CPD      = OFF
#pragma config BOREN    = ON
#pragma config CLKOUTEN = OFF
#pragma config IESO     = OFF
#pragma config FCMEN    = OFF

#pragma config WRT      = OFF
#pragma config PLLEN    = OFF
#pragma config STVREN   = ON
#pragma config BORV     = HI
#pragma config LVP      = OFF


#define _XTAL_FREQ 4000000


/* Initialize basic config. */
void init();


#define clearWatchdog()            { asm("CLRWDT"); }
#define wasWatchdogReset()         ( __timeout == 0 ) //this is ~TO - valid only if +resetbits is set on linker

#define wait_100ms()               { __delay_ms(100); }
#define waitWithWatchdog_1000ms()  { for (uint8_t i=0; i<10; i++) { clearWatchdog(); wait_100ms(); } }

#endif
