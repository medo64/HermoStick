#include "config.h"
#include "ds2482.h"
#include "ds1820.h"
#include "i2c.h"
#include "io.h"
#include "uart.h"


void main() {
    init();
    io_init();
    uart_init(9600);
    i2c_init();

    wait_100ms(); io_activity_active(); wait_100ms(); io_activity_default();
    wait_100ms(); io_activity_active(); wait_100ms(); io_activity_default();
    wait_100ms(); io_activity_active(); wait_100ms(); io_activity_default();

    if (wasWatchdogReset()) {
        wait_100ms(); io_activity_active(); wait_100ms(); io_activity_default(); //two more flashes if there was a WDT reset
        wait_100ms(); io_activity_active(); wait_100ms(); io_activity_default();
        uart_writeConstBytes("{E:WDT}\r\n");
    }

    
    while (true) {
        clearWatchdog();

        if (!ds2482_deviceReset()) { //Reset device
            uart_writeConstBytes("{E:DRST}\r\n");
            continue;
        }


        if (!ds2482_writeConfiguration(0b11110000)) { //Write configuration: 1WS,SPU,0,APU
            uart_writeConstBytes("{E:WCFG}\r\n");
            continue;
        }


        if (!ds2482_busDetectPresence()) { //Detect presence
            uart_writeConstBytes("{E:PRESENCE}\r\n");
            continue;
        }


        ds1820_resetAndConvertTForAll();
        waitWithWatchdog_1000ms();


        ds2482_romSearchInit();
        uint8_t romCode[8];
        while (ds2482_romSearchNext(romCode)) {
            clearWatchdog();
            for (uint8_t i=0; i<8; i++) { uart_writeByteAsHex(romCode[i]); }

            io_activity_active();

            ds1820_resetAndMatchRom(romCode);

            uint8_t scratchpad[9];
            bool success = ds1820_readScratchpadBytes(&scratchpad, 9);
            io_activity_default();

            if (success) {
                for (uint8_t i=0; i<9; i++) { uart_writeByteAsHex(scratchpad[i]); }
                uart_writeConstBytes("\r\n");
            } else {
                uart_writeConstBytes("{E:SCRATCHPAD}\r\n");
            }
        }


        uart_writeConstBytes("\r\n");
    }
}
