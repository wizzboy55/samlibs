/*
 * i2c.c
 *
 * Created: 2019-05-22 10:36:08 AM
 *  Author: Wizz
 */ 

#include "i2c.h"

#define I2C_READMASK 0x01

#ifdef SERCOM_I2CM_CTRLA_LOWTOUT
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUT
#elif defined (SERCOM_I2CM_CTRLA_LOWTOUTEN)
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUTEN
#endif

void i2c_master_initIF(Sercom * sercom, const uint8_t clk_channel, uint32_t baudrate, uint8_t pin_sda, uint8_t pin_scl, uint8_t mux) {
//	_pm_enable_bus_clock(PM_BUS_APBC, sercom);
//	_gclk_enable_channel(clk_channel, GCLK_CLKCTRL_GEN_GCLK0_Val);

	sercom->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;

	// Wait while reset is in effect
	while(sercom->I2CM.CTRLA.bit.SWRST) {

	}

	sercom->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;

	sercom->I2CM.BAUD.reg = (CONF_CPU_FREQUENCY / (2 * baudrate)) - 1;

	sercom->I2CM.INTENCLR.reg = 0xFF;

	sercom->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_LOWTOUT_Val | SERCOM_I2CM_CTRLA_ENABLE | SERCOM_I2CM_CTRLA_MODE(0x04);

	sercom->I2CM.STATUS.bit.BUSSTATE = 0x01;

	gpio_set_pin_function(pin_sda, mux);
	gpio_set_pin_function(pin_scl, mux);
}

uint8_t i2c_master_readByte(Sercom * sercom, uint8_t device) {
	sercom->I2CM.CTRLB.bit.ACKACT = 0;

	sercom->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercom->I2CM.INTFLAG.bit.SB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.CTRLB.bit.CMD = 0x03;	// Send NACK

	return sercom->I2CM.DATA.reg;
}

uint8_t i2c_master_readBytes(Sercom * sercom, uint8_t device, uint8_t *buf, uint16_t num) {
	sercom->I2CM.CTRLB.bit.ACKACT = 0;

	sercom->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercom->I2CM.INTFLAG.bit.SB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	uint16_t i;

	for(i = 0; i < num; i++) {
		if(i < (num - 1)) {
			sercom->I2CM.CTRLB.bit.ACKACT = 0;
			buf[i] = sercom->I2CM.DATA.reg;
			sercom->I2CM.CTRLB.bit.CMD = 0x02;	// Send ACK
			} else {
			sercom->I2CM.CTRLB.bit.ACKACT = 1;
			sercom->I2CM.CTRLB.bit.CMD = 0x03;	// Send NACK
			buf[i] = sercom->I2CM.DATA.reg;
			break;
		}
		uint16_t timeout = 0xFFFF;
		while(sercom->I2CM.INTFLAG.bit.SB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1) {
			if(timeout-- == 0) {
				return 0;
			}
		}
	}

	return i;
}

uint8_t i2c_master_writeAddr(Sercom * sercom, uint8_t device, uint8_t addr) {
	sercom->I2CM.CTRLB.bit.ACKACT = 0;

	sercom->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.DATA.reg = addr;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeByte(Sercom * sercom, uint8_t device, uint8_t addr, uint8_t data) {
	sercom->I2CM.CTRLB.bit.ACKACT = 0;

	sercom->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.DATA.reg = addr;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.DATA.reg = data;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeBytes(Sercom * sercom, uint8_t device, uint8_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	sercom->I2CM.CTRLB.bit.ACKACT = 0;

	sercom->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	sercom->I2CM.DATA.reg = addr;

	while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercom->I2CM.DATA.reg = data[bytesSent];

		while(sercom->I2CM.INTFLAG.bit.MB != 1 && sercom->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercom->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercom->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}