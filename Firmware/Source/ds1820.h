#ifndef DS1820_H
#define DS1820_H

#include <stdbool.h>
#include <stdint.h>


/* Executes Match ROM command. */
void ds1820_resetAndMatchRom(uint8_t* romCode);

/* Executes Convert T command on all devices. */
void ds1820_resetAndConvertTForAll();

/* Executes Read Scratchpad command. */
bool ds1820_readScratchpadBytes(uint8_t* value, uint8_t count);


#endif
