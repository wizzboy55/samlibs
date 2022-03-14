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
	uint16_t timeout;
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

#endif /* I2C_H_ */