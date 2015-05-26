#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>


/** Initializes UART peripheral. */
void uart_init(uint16_t desiredBaudRate);


/** Returns true if there is data waiting to be read. */
bool uart_canRead();

/** Returns true if there is no data currently being written. */
bool uart_canWrite();


/** Blocking read of a single byte. */
uint8_t uart_readByte();

/** Blocking write of a single byte. */
void uart_writeByte(uint8_t data);

/** Blocking write of a single byte. */
void uart_writeByteAsHex(uint8_t data);


/** Blocking writes of a multiple bytes. */
void uart_writeBytes(uint8_t* data, uint8_t count);

/** Blocking writes of a multiple bytes until the null char. */
void uart_writeConstBytes(const uint8_t* data);

#endif
