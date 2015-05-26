#ifndef I2C_H
#define I2C_H

#include <stdbool.h>
#include <stdint.h>


#define I2C_READ  1  /* Read operation. */
#define I2C_WRITE 0  /* Write operation. */


/* Initialize I2C peripheral. */
void i2c_init();


/* Initiates start condition on I2C bus. */
void i2c_start();

/* Initiates repeated start condition on I2C bus. */
void i2c_restart();

/* Initiates stop condition on I2C bus. */
void i2c_stop();


/* Sets address of I2C device and it's operation. */
bool i2c_executeRead(uint8_t address);

/* Sets address of I2C device and it's operation. */
bool i2c_executeWrite(uint8_t address);


/* Writes one byte to I2C bus. */
bool i2c_writeByte(uint8_t data);

/* Returns TRUE if all bytes were successfully read. */
bool i2c_readBytes(uint8_t *data, uint8_t count);

#endif
