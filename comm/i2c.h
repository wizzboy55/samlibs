/*
 * i2c.h
 *
 * Created: 2019-05-22 10:36:02 AM
 *  Author: Wizz
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "FreeRTOS.h"

#include "stdint.h"
#include "sercom.h"

typedef struct {
	void* sercom;
	uint32_t baudrate;
	uint8_t pin_sda;
	uint8_t pin_scl;
	uint8_t pinmux;
	uint8_t clksource;
} i2cConfig_t;

void i2c_master_initIF(i2cConfig_t* config);
uint8_t i2c_master_readByte(i2cConfig_t* config, uint8_t device);
uint8_t i2c_master_readBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num);
uint8_t i2c_master_readBytes_BE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num);
uint8_t i2c_master_writeAddr_LE(i2cConfig_t* config, uint8_t device, uint8_t addr);
uint8_t i2c_master_writeAddr16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr);
uint8_t i2c_master_writeAddr16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr);
uint8_t i2c_master_writeByte(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t data);
uint8_t i2c_master_writeBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t *data, uint16_t num);
uint8_t i2c_master_writeBytes16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num);
uint8_t i2c_master_writeBytes16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num);



/*
	Write multiple bytes to the I2C bus

	- Sends a start condition if the bus is IDLE or repeat start if the bus is OWNED by
	- Writes the slave_addr and read/write flag
	- Writes the buffer and validate the ACK for each byte
	- Does not end the transaction automatically (no stop bit sent)

	Returns the number of bytes written
*/
uint16_t i2c_mst_write(i2cConfig_t* config, uint8_t slave_addr, uint8_t *buf, uint8_t buf_size);

/*
	Read multiple bytes on the I2C bus

	- Sends a start condition if the bus is IDLE or repeat start if the bus is OWNED by
	- Writes the slave_addr and read/write flag
	- Writes the buffer and validate the ACK for each byte and NACK for the last byte
	- Does not end the transaction automatically (no stop bit sent)

	Returns the number of bytes read
*/
uint16_t i2c_mst_read(i2cConfig_t* config, uint8_t slave_addr, uint8_t *buf, uint8_t buf_size);

// Read one byte
uint8_t i2c_mst_readByte(i2cConfig_t* config, uint8_t slave_addr);

uint16_t i2c_mst_stop(i2cConfig_t* config);


#endif /* I2C_H_ */