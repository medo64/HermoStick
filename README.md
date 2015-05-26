### Hermo ###

Interface for reading 1-wire temperature sensors (e.g. DS18B20 and DS18S20).


#### Serial Parameters ####

9600,N,8,1


#### Data Format ####

Data is returned as a hexadecimal string, terminated by <CR><LF>. All numbers
are in little-endian order.

| Offset | Length | Field                  |
|-------:|-------:|------------------------|
|      0 |      2 | Family                 |
|      2 |     12 | Serial                 |
|     14 |      2 | CRC (ID)               |
|     16 |      4 | Temperature            |
|     20 |      2 | User byte 1            |
|     22 |      2 | User byte 2            |
|     24 |      4 | Reserved bytes         |
|     28 |      2 | Count remaining        |
|     30 |      2 | Counts per °C          |
|     32 |      2 | CRC (Scratchpad)       |

Rows not following given format (including empty rows) are to be ignored..


##### Examples #####

###### DS18S20 ######

|                   | `109A29130000003E09000705FFFF184EC4` |
|-------------------|--------------------------------------|
| Family:           | `10`                                 |
| Serial:           | `00000013299A`                       |
| CRC (ID):         | `3E`                                 |
| Temperature:      | `0009`                               |
| User byte 1:      | `07`                                 |
| User byte 2:      | `05`                                 |
| Reserved bytes:   | `FF` `FF`                            |
| Count remaining:  | `18`                                 |
| Counts per °C:    | `4E`                                 |
| CRC (Scratchpad): | `C4`                                 |

Temperature is read in 0.5 °C. Sample temperature is 4.5 °C.


###### DS18B20 ######

|                   | `2824F76B000000872C014B467FFF041014` |
|-------------------|--------------------------------------|
| Family:           | `28`                                 |
| Serial:           | `0000006BF724`                       |
| CRC (ID):         | `87`                                 |
| Temperature:      | `012C`                               |
| User byte 1:      | `4B`                                 |
| User byte 2:      | `46`                                 |
| Reserved bytes:   | `7F` `FF`                            |
| Count remaining:  | `04`                                 |
| Counts per °C:    | `10`                                 |
| CRC (Scratchpad): | `14`                                 |

Temperature is read in 0.0625 °C. Sample temperature is 18.75 °C.


##### Errors #####

Errors are included in stream in brackets (`{``}`) and prefixed by `E:`, e.g.:

    {E:PRESENCE}

| Error      | Description                               |
|------------|-------------------------------------------|
| DRST       | Cannot reset 1-wire master (DS2482)       |
| WCFG       | Cannot configure 1-wire master (DS2482)   |
| PRESENCE   | No 1-wire devices detected                |
| SCRATCHPAD | Cannot read scratchpad data               |
| WDT        | Device restarted due to watchdog timeout. |


#### Connector ####

RJ-45 jacks are used for connecting temperature sensors to Hermo. Alternatively
a 3-pin KK100 connector can be used.


##### RJ-45 pinout #####

| # | 1-wire | T568A  colors |
|---|--------|---------------|
| 1 | -      | green/white   |
| 2 | -      | green         |
| 3 | (GND)  | orange/white  |
| 4 | DQ     | blue          |
| 5 | GND    | blue/white    |
| 6 | 5V     | orange        |
| 7 | -      | brown/white   |
| 8 | -      | brown         |


##### 3-pin Molex pinout #####

|   | 1-wire |
|---|--------|
| 1 | GND    |
| 2 | DQ     |
| 3 | 5V     |
