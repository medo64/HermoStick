#include <xc.h>
#include <stdbool.h>
#include "uart.h"
#include "i2c.h"


#define I2C_DEBUG 0


void i2c_init() {
    TRISB1 = 1;   //PORTB pin configured as an input (tri-stated)
    TRISB4 = 1;   //PORTB pin configured as an input (tri-stated)
    SSPEN  = 1;   //Enables the serial port and configures the SDAx and SCLx pins as the source of the serial port pins
    SSPM3  = 1;   //I2C Master mode, clock = FOSC / (4 * (SSPxADD+1))
    SSPM2  = 0;
    SSPM1  = 0;
    SSPM0  = 0;
    SMP    = 1;   //Slew rate control disabled for standard speed mode (100 kHz and 1 MHz)
    SSPADD = 9;   //100 kHz
}

void i2c_wait() {
    while (SSP1IF == 0);
    SSP1IF = 0;
}


void i2c_start() {
    #if I2C_DEBUG
        uart_writeConstBytes("S");
    #endif
    SEN = 1;   //Initiate Start condition on SDAx and SCLx pins. Automatically cleared by hardware.
    i2c_wait();
}


void i2c_restart() {
    #if I2C_DEBUG
        uart_writeConstBytes("R");
    #endif
    RSEN = 1;   //Initiate Repeated Start condition on SDAx and SCLx pins. Automatically cleared by hardware.
    i2c_wait();
}


void i2c_stop() {
    #if I2C_DEBUG
        uart_writeConstBytes("P");
    #endif
    PEN = 1;   //Initiate Stop condition on SDAx and SCLx pins. Automatically cleared by hardware
    i2c_wait();
}


bool i2c_execute(uint8_t address, bool isRead) {
    #if I2C_DEBUG
        uart_writeConstBytes("a");
        uart_writeByteAsHex((address << 1) | (isRead & 0x01));
    #endif
    SSP1BUF = (address << 1) | (isRead ? 0x01 : 0x00);
    i2c_wait();
    if (ACKSTAT == 1) {
        #if I2C_DEBUG
            uart_writeConstBytes("!");
        #endif
        return false;
    }
    return true;
}


bool i2c_executeRead(uint8_t address) {
    return i2c_execute(address, true);
}


bool i2c_executeWrite(uint8_t address) {
    return i2c_execute(address, false);
}


bool i2c_writeByte(uint8_t value) {
    #if I2C_DEBUG
        uart_writeConstBytes("w");
        uart_writeByteAsHex(value);
    #endif
    SSP1BUF = value;
    i2c_wait();
    if (ACKSTAT == 1) {
        #if I2C_DEBUG
            uart_writeConstBytes("!");
        #endif
        return false;
    }
    return true;
}


bool i2c_readBytes(uint8_t* value, uint8_t count) {
    #if I2C_DEBUG
        uart_writeConstBytes("r");
    #endif
    for (uint8_t i=0; i<count; i++) {
        RCEN = 1;
        i2c_wait();
        *value = SSP1BUF;
        BF = 0; //Data transmit complete (does not include the ACK  and Stop bits), SSPxBUF is empty
        if (ACKSTAT == 1) {
            #if I2C_DEBUG
                uart_writeConstBytes("!");
            #endif
            return false;
        }
        value++;
        #if I2C_DEBUG
            uart_writeByteAsHex(*value);
        #endif
        ACKDT = (i<count-1) ? 0 : 1; //NACK for last byte
        ACKEN = 1; //Initiate Acknowledge sequence on SDAx and SCLx pins, and transmit ACKDT data bit. Automatically cleared by hardware.
        i2c_wait();
    }
    return true;
}
