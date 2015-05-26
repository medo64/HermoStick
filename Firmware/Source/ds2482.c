#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "ds2482.h"
#include "i2c.h"


#define OWM_ADDR  0b0011000

#define OWM_CMD_1WRB  0x96  /* 1-Wire Read Byte */
#define OWM_CMD_1WWB  0xA5  /* 1-Wire Write Byte */
#define OWM_CMD_1WRS  0xB4  /* 1-Wire Reset */
#define OWM_CMD_WCFG  0xD2  /* Write Configuration */
#define OWM_CMD_SRP   0xE1  /* Set Read Pointer */
#define OWM_CMD_DRST  0xF0  /* Device Reset */
#define OWM_CMD_1WT   0x78  /* 1-Wire Triplet */

#define OWM_REG_CONFIGURATION  0xC3
#define OWM_REG_DATA           0xE1
#define OWM_REG_STATUS         0xF0

#define OWM_STATUS_BITMASK_1WB  0x01 //1-Wire Busy
#define OWM_STATUS_BITMASK_PPD  0x02 //Presence-Pulse Detect
#define OWM_STATUS_BITMASK_SD   0x04 //Short Detected
#define OWM_STATUS_BITMASK_LL   0x08 //Logic Level
#define OWM_STATUS_BITMASK_RST  0x10 //Device Reset
#define OWM_STATUS_BITMASK_SBR  0x20 //Single Bit Result
#define OWM_STATUS_BITMASK_TSB  0x40 //Triplet Second Bit
#define OWM_STATUS_BITMASK_DIR  0x80 //Branch Direction Taken

#define OW_CMD_SEARCH_ROM  0xF0


#define owm_wait() __delay_ms(10);


uint8_t lastRomCode[8];
uint8_t lastDiscrepancy;
bool lastDeviceFlag;

uint8_t romSearchTriplet(uint8_t searchDirection) {
    i2c_start();
    i2c_executeWrite(OWM_ADDR);
    i2c_writeByte(OWM_CMD_1WT);
    i2c_writeByte(searchDirection ? 0x80 : 0x00);
    i2c_stop();

    owm_wait();

    i2c_start();
    i2c_executeRead(OWM_ADDR);
    uint8_t status;
    i2c_readBytes(&status, 1);
    i2c_stop();

    if ((status & OWM_STATUS_BITMASK_1WB) == OWM_STATUS_BITMASK_1WB) { //check whether device is still busy
        return UINT8_MAX;
    } else {
        return status;
    }
}


void ds2482_busReset() {
    i2c_start();
    i2c_executeWrite(OWM_ADDR);
    i2c_writeByte(OWM_CMD_1WRS); //1-Wire Reset
    i2c_stop();
    owm_wait()
}


bool ds2482_busDetectPresence() {
    ds2482_busReset();

    i2c_start();
    i2c_executeRead(OWM_ADDR);
    uint8_t dataPresence;
    bool success = i2c_readBytes(&dataPresence, 1);
    i2c_stop();

    if (!success) { return false; }
    if ((dataPresence & OWM_STATUS_BITMASK_PPD) != OWM_STATUS_BITMASK_PPD) { return false; }
    return true;
}


void ds2482_busWriteByte(uint8_t data) {
    i2c_start();
    i2c_executeWrite(OWM_ADDR);
    i2c_writeByte(OWM_CMD_1WWB);
    i2c_writeByte(data);
    i2c_stop();
    owm_wait()
}


bool ds2482_busReadBytes(uint8_t* data, uint8_t count) {
    for (uint8_t i=0; i<count; i++) {
        i2c_start();
        i2c_executeWrite(OWM_ADDR);
        i2c_writeByte(OWM_CMD_1WRB);
        i2c_stop();
        owm_wait();

        i2c_start();
        i2c_executeWrite(OWM_ADDR);
        i2c_writeByte(OWM_CMD_SRP);
        i2c_writeByte(OWM_REG_DATA);
        i2c_stop();
        owm_wait();

        i2c_start();
        i2c_executeRead(OWM_ADDR);
        bool okRead = i2c_readBytes(data, 1);
        i2c_stop();
        owm_wait();

        if (!okRead) { return false; }
        data++;
    }
    return true;
}


bool ds2482_deviceReset() {
    i2c_start();
    i2c_executeWrite(OWM_ADDR);
    bool success = i2c_writeByte(OWM_CMD_DRST); //Device Reset
    i2c_stop();

    return success;
}


bool ds2482_writeConfiguration(uint8_t configurationBits) {
    i2c_start();
    i2c_executeWrite(OWM_ADDR);
    bool success = i2c_writeByte(OWM_CMD_WCFG) && i2c_writeByte(configurationBits);
    i2c_stop();

    return success;
}


void ds2482_romSearchInit() {
    lastDiscrepancy = 0;
    lastDeviceFlag = false;
}

bool ds2482_romSearchNext(uint8_t* romCode) {
    if (lastDeviceFlag) { return false; } //no more devices, do RomSearchInit

    // initialize for search
    uint8_t lastZeroBitIndex = 0;
    uint8_t bitIndex = 1;
    uint8_t byteIndex = 0;
    uint8_t byteMask = 0x01;

    ds2482_busReset();
    ds2482_busWriteByte(OW_CMD_SEARCH_ROM);

    uint8_t searchDirection;
    do {
        if (bitIndex < lastDiscrepancy) { // if this discrepancy if before the Last Discrepancy on a previous next then pick the same as last time
            searchDirection = ((lastRomCode[byteIndex] & byteMask) > 0) ? 1 : 0;
        } else { // if equal to last pick 1, if not then pick 0
            searchDirection = (bitIndex == lastDiscrepancy) ? 1 : 0;
        }

        uint8_t status = romSearchTriplet(searchDirection);

        uint8_t sbrBit = ((status & OWM_STATUS_BITMASK_SBR) == OWM_STATUS_BITMASK_SBR);
        uint8_t tsbBit = ((status & OWM_STATUS_BITMASK_TSB) == OWM_STATUS_BITMASK_TSB);
        searchDirection = ((status & OWM_STATUS_BITMASK_DIR) == OWM_STATUS_BITMASK_DIR) ? 1 : 0;

        if (sbrBit && tsbBit) { //check for no devices on 1-Wire
            break;
        } else {
            if (!sbrBit && !tsbBit && (searchDirection == 0)) {
                lastZeroBitIndex = bitIndex;
            }

            if (searchDirection == 0) {
                lastRomCode[byteIndex] &= ~byteMask;
            } else {
                lastRomCode[byteIndex] |= byteMask;
            }

            bitIndex += 1;
            byteMask <<= 1;
            if (byteMask == 0) {
                byteIndex += 1;
                byteMask = 0x01;
            }
        }
    } while (byteIndex < 8); // loop until through all ROM bytes 0-7

    if (!(bitIndex < 65)) { // if the search was successful then
        lastDiscrepancy = lastZeroBitIndex;
        if (lastDiscrepancy == 0) { lastDeviceFlag = true; }

        for (uint8_t i=0; i<8; i++) {
            romCode[i] = lastRomCode[i];
        }
        return true;
    } else {
        return false;
    }
}
