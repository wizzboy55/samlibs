/*
 * i2c.c
 *
 * Created: 2019-05-22 10:36:08 AM
 *  Author: Wizz
 */ 

#include "i2c.h"
#include "samclk.h"
#include "samgpio.h"

#define I2C_READMASK 0x01

#define SERCOM_I2CM_CTRLA_MODE_I2CM 0x05

#ifdef SERCOM_I2CM_CTRLA_LOWTOUT
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUT
#elif defined (SERCOM_I2CM_CTRLA_LOWTOUTEN)
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUTEN
#endif

void i2c_master_initIF(i2cConfig_t* config) {

	Sercom* sercomdevice = (Sercom *)config->sercom;

	if(sercomdevice->I2CM.CTRLA.bit.MODE == SERCOM_I2CM_CTRLA_MODE_I2CM) {
		return;
	}

	samclk_enable_peripheral_clock(sercomdevice);
	samclk_enable_gclk_channel(sercomdevice, config->clksource);
	samclk_enable_glck_slow_channel(sercomdevice, 11);

	sercomdevice->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;

	// Wait while reset is in effect
	while(sercomdevice->I2CM.CTRLA.bit.SWRST) {}

	sercomdevice->I2CM.CTRLA.bit.MODE = SERCOM_I2CM_CTRLA_MODE_I2CM;

	sercomdevice->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN; // | SERCOM_I2CM_CTRLB_ACKACT;
	
	sercomdevice->I2CM.CTRLA.bit.LOWTOUTEN = 1;
	
	sercomdevice->I2CM.CTRLA.bit.INACTOUT = 1;

	// We are assuming GCLK source clock is the main clock (CONF_CPU_FREQUENCY) divided by DIV to calculate the baud
	uint8_t div = GCLK->GENCTRL[config->clksource].bit.DIV == 0 ? 1 : GCLK->GENCTRL[config->clksource].bit.DIV; // Ensure != 0
	uint16_t baud = CONF_CPU_FREQUENCY / div / (2 * config->baudrate) - 1;
	ASSERT(baud <= 0xff); // Invalid baudrate for the chosen clock source
	sercomdevice->I2CM.BAUD.reg = baud;

	sercomdevice->I2CM.INTENCLR.reg = 0xFF;

	sercomdevice->I2CM.CTRLA.bit.ENABLE = 1;

	sercomdevice->I2CM.STATUS.bit.BUSSTATE = 0x01;
	while(sercomdevice->I2CM.SYNCBUSY.bit.SYSOP == 1);

	samgpio_setPinFunction(config->pin_sda, config->pinmux);
	samgpio_setPinFunction(config->pin_scl, config->pinmux);
}

uint8_t i2c_master_readByte(i2cConfig_t* config, uint8_t device) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
	
	sercomdevice->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Acknowledgment Action + Stop

	return sercomdevice->I2CM.DATA.reg;
}

uint8_t i2c_master_readBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	uint16_t i;

	for(i = 0; i < num; i++) {
		if(i < (num - 1)) {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
			buf[i] = sercomdevice->I2CM.DATA.reg;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x02;	// Send ACK
			} else {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 1;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send NACK + Stop
			buf[i] = sercomdevice->I2CM.DATA.reg;
			break;
		}
		uint16_t timeout = 0xFFFF;
		while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1) {
			if(timeout-- == 0) {
				return 0;
			}
		}
	}

	return i;
}

uint8_t i2c_master_readBytes_BE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
	sercomdevice->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR);

	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return 0;
	}

	uint16_t i;

	for(i = 0; i < num; i++) {
		if(i < (num - 1)) {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
			buf[num-i-1] = sercomdevice->I2CM.DATA.reg;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x02;	// Send ACK
		} else {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 1;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send NACK + Stop
			buf[num-i-1] = sercomdevice->I2CM.DATA.reg;
			break;
		}
		uint16_t timeout = 0xFFFF;
		while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1) {
			if(timeout-- == 0) {
				return 0;
			}
		}
	}

	return i;
}

uint8_t i2c_master_writeAddr_LE(i2cConfig_t* config, uint8_t device, uint8_t addr) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR);

	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return pdFALSE;
	}

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR);
	
	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return pdFALSE;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeAddr16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);
	
	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeAddr16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);
	
	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeByte(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t data) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
	
	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = data;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);
	
	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercomdevice->I2CM.DATA.reg = data[bytesSent];

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeBytes16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);
	
	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercomdevice->I2CM.DATA.reg = data[bytesSent];

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}

uint8_t i2c_master_writeBytes16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);
	
	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);
	
	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercomdevice->I2CM.DATA.reg = data[num-bytesSent-1];

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}