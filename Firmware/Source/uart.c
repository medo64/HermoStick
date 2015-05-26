#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#include "config.h"


void resetRx() {
    CREN = 0; //disable continous receive, also clears errors
    CREN = 1; //enable continous receive
    uint8_t dummyRead;
    dummyRead = RCREG;
    dummyRead = RCREG;
    dummyRead = RCREG; //read data to clear FIFO
    SPEN = 0; //disable USART.
    SPEN = 1; //enable USART.
}


void uart_init(uint16_t desiredBaudRate) { //must be 19200 or less
    //USART
    ANSB1   = 0;    //Digital I/O. Pin is assigned to port or digital special function.
    TRISB2  = 1;    //PORTB pin configured as an input (tri-stated)
    RXDTSEL = 1;    //RX/DT function is on RB2
    TXCKSEL = 1;    //TX/CK function is on RB5
    SPBRG   = (uint8_t)(_XTAL_FREQ / desiredBaudRate / 16 - 1);
    BRGH    = 1;    //High speed
    SYNC    = 0;    //Asynchronous mode
    SPEN    = 1;    //Serial port enabled (configures RX/DT and TX/CK pins as serial port pins)
    TXEN    = 1;    //Transmit enabled
    CREN    = 1;    //Enables receiver
    resetRx();
}

bool uart_canRead() {
    return (RCIF == 0) ? false : true;
}

bool uart_canWrite() {
    return (TXIF == 0) ? false : true;
}

uint8_t uart_readByte() {
    if (FERR) { resetRx(); } //framing error
    if (OERR) { resetRx(); } //overrun error
    while (RCIF == 0) { } //wait until something is received
    return RCREG;
}

void uart_writeByte(uint8_t data) {
    while (TXIF == 0); //wait until buffer is empty
    TXREG = data;
}

void uart_writeBytes(uint8_t* data, uint8_t count) {
    for (uint8_t i=0; i<count; i++) {
        while (TXIF == 0); //wait until buffer is empty
        TXREG = data[i];
    }
}

void uart_writeConstBytes(const uint8_t* data) {
    while (*data != '\0') {
        while (TXIF == 0) { } //wait until buffer is empty
        TXREG = *data;
        data++;
    }
}

void uart_writeByteAsHex(uint8_t data) {
    uint8_t hex[2];
    hex[0] = 0x30 + (data >> 4);
    hex[1] = 0x30 + (data & 0x0F);
    if (hex[0] > 0x39) { hex[0] += 7; }
    if (hex[1] > 0x39) { hex[1] += 7; }
    uart_writeBytes(hex, 2);
}
