#ifndef DS2482_H
#define DS2482_H

#include <stdbool.h>
#include <stdint.h>


/* Returns true if device was successfully reset. */
bool ds2482_deviceReset();

/* Returns true if configuration was successfully written. */
bool ds2482_writeConfiguration(uint8_t configurationBits);

/* Initializes search procedure. */
void ds2482_romSearchInit();

/* Returns true if device is found and places it's ROM code into romCode array. */
bool ds2482_romSearchNext(uint8_t* romCode);


/* Resets 1-wire bus. */
void ds2482_busReset();

/* Returns true if any 1-wire  device is present */
bool ds2482_busDetectPresence();

/* Writes single byte to 1-wire bus. */
void ds2482_busWriteByte(uint8_t data);

/* Returns true if reading count characters from 1-wire bus was successful. */
bool ds2482_busReadBytes(uint8_t* data, uint8_t count);


#endif
