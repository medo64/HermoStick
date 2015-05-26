#include "ds2482.h"


#define DS1820_CMD_READ_ROM         0x33  /* READ_ROM command. */
#define DS1820_CMD_MATCH_ROM        0x55  /* MATCH_ROM command. */
#define DS1820_CMD_SKIP_ROM         0xCC  /* SKIP_ROM command. */
#define DS1820_CMD_CONVERT_T        0x44  /* CONVERT_T command. */
#define DS1820_CMD_READ_SCRATCHPAD  0xBE  /* READ_SCRATCHPAD command. */


void ds1820_resetAndMatchRom(uint8_t* romCode) {
    ds2482_busReset();
    ds2482_busWriteByte(DS1820_CMD_MATCH_ROM);
    ds2482_busWriteByte(romCode[0]);
    ds2482_busWriteByte(romCode[1]);
    ds2482_busWriteByte(romCode[2]);
    ds2482_busWriteByte(romCode[3]);
    ds2482_busWriteByte(romCode[4]);
    ds2482_busWriteByte(romCode[5]);
    ds2482_busWriteByte(romCode[6]);
    ds2482_busWriteByte(romCode[7]);
}


void ds1820_resetAndConvertTForAll() {
    ds2482_busReset();
    ds2482_busWriteByte(DS1820_CMD_SKIP_ROM); //get all devices to convert temperature at same time
    ds2482_busWriteByte(DS1820_CMD_CONVERT_T);
}


bool ds1820_readScratchpadBytes(uint8_t *value, uint8_t count) {
    ds2482_busWriteByte(DS1820_CMD_READ_SCRATCHPAD);
    return ds2482_busReadBytes(value, count);
}
