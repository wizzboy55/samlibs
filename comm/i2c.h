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
#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#include <hal_gpio.h>
#include <hal_io.h>

void i2c_master_initIF(Sercom * sercom, const uint8_t clk_channel, uint32_t baudrate, uint8_t pin_sda, uint8_t pin_scl, uint8_t mux);
uint8_t i2c_master_readByte(Sercom * sercom, uint8_t device);
uint8_t i2c_master_readBytes(Sercom * sercom, uint8_t device, uint8_t *buf, uint16_t num);
uint8_t i2c_master_writeAddr(Sercom * sercom, uint8_t device, uint8_t addr);
uint8_t i2c_master_writeByte(Sercom * sercom, uint8_t device, uint8_t addr, uint8_t data);
uint8_t i2c_master_writeBytes(Sercom * sercom, uint8_t device, uint8_t addr, uint8_t *data, uint16_t num);

#endif /* I2C_H_ */