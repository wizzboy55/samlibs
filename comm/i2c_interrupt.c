/*
 * i2c_interrupt.c
 *
 * Created: 2022-03-19 10:00:16 AM
 *  Author: charl
 */ 

#include "i2c_interrupt.h"

#include "samclk.h"
#include "samgpio.h"
#include "saminterrupt.h"

#define I2C_STACK_SIZE	256

#define I2C_READMASK 0x01
#define SERCOM_I2CM_CTRLA_MODE_I2CM 0x05

enum I2cmCmd_e {I2cmCmd_NoAction, I2cmCmd_AckStart, I2cmCmd_AckRead, I2cmCmd_AckStop};
enum I2cmErrors_e {I2cmError_None, I2cmError_ArbLost, I2cmError_BusError, I2cmError_NoSlave};

void vI2cInterruptTask(void* p) {
	I2CInterruptConfig_t* config = (I2CInterruptConfig_t*)p;
	
	Sercom* sercomdevice = (Sercom*)config->sercom;
	
	config->xI2cTask = xTaskGetCurrentTaskHandle();

	samclk_enable_peripheral_clock(sercomdevice);
	samclk_enable_gclk_channel(sercomdevice, config->clksource);
	samclk_enable_glck_slow_channel(sercomdevice, 11);

	sercomdevice->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;

	// Wait while reset is in effect
	while(sercomdevice->I2CM.CTRLA.bit.SWRST) {

	}

	sercomdevice->I2CM.CTRLA.bit.MODE = SERCOM_I2CM_CTRLA_MODE_I2CM;
		
	sercomdevice->I2CM.CTRLA.bit.LOWTOUTEN = 1;
		
	sercomdevice->I2CM.CTRLA.bit.INACTOUT = 3;
	
	sercomdevice->I2CM.CTRLA.bit.MEXTTOEN = 1;
	
	sercomdevice->I2CM.CTRLA.bit.SDAHOLD = 0;

	sercomdevice->I2CM.BAUD.reg = (CONF_CPU_FREQUENCY / (2 * config->baudrate)) - 1;

	sercomdevice->I2CM.INTENCLR.reg = 0xFF;

	samgpio_setPinFunction(config->pin_sda, config->pinmux);
	samgpio_setPinFunction(config->pin_scl, config->pinmux);
	
	config->xState = I2C_IDLE;
	
	sam_setModuleSubInterruptPriority(config->sercom, SERCOM_I2CM_INTFLAG_MB_Pos, config->ucInterruptPriority);
	sam_setModuleSubInterruptPriority(config->sercom, SERCOM_I2CM_INTFLAG_SB_Pos, config->ucInterruptPriority);
	sam_setModuleSubInterruptPriority(config->sercom, 2, config->ucInterruptPriority);
	sam_setModuleSubInterruptPriority(config->sercom, 3, config->ucInterruptPriority);
	
	sercomdevice->I2CM.CTRLA.bit.ENABLE = 1;
	
	for(;;) {		
		if(xQueueReceive(config->xTransactionQueue, &(config->pxCurrentTransaction), portMAX_DELAY)) {
			
			config->pxCurrentTransaction->xTransactionSuccess = pdFALSE;
			
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
			sercomdevice->I2CM.STATUS.bit.BUSSTATE = 0x01;
			if(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01) {
//				sercomdevice->I2CM.CTRLA.bit.ENABLE = 0;
//				sercomdevice->I2CM.CTRLA.bit.ENABLE = 1;
			}
			uint8_t deviceAddress;
			switch(config->pxCurrentTransaction->xTransactionType) {
				case I2C_WRITE:
					config->xState = I2C_WRITE_REGISTER;
					break;
				case I2C_READ:
					config->xState = I2C_READ_DATA;
					break;
				case I2C_WRITEREAD:
					config->xState = I2C_WRITE_REGISTER;
					break;
			}
			switch(config->xState) {
				case I2C_WRITE_REGISTER:
				case I2C_WRITE_DATA:
					deviceAddress = config->pxCurrentTransaction->ucDeviceAddress & ~I2C_READMASK;
					sercomdevice->I2CM.INTENSET.bit.MB = 1;
					sercomdevice->I2CM.INTENSET.bit.SB = 1;
					break;
				case I2C_READ_DATA:
					deviceAddress = config->pxCurrentTransaction->ucDeviceAddress | I2C_READMASK;
					sercomdevice->I2CM.INTENSET.bit.SB = 1;
					break;
				default:
					continue;
			}
			sercomdevice->I2CM.INTENSET.bit.ERROR = 0;
			sam_enableModuleSubInterrupt(config->sercom, SERCOM_I2CM_INTFLAG_MB_Pos);
			sam_enableModuleSubInterrupt(config->sercom, SERCOM_I2CM_INTFLAG_SB_Pos);
			sercomdevice->I2CM.ADDR.reg = deviceAddress;
			uint32_t driverNotification = 0;	
			if(xTaskNotifyWait(0xFFFFFFFF, 0, &driverNotification, config->xTimeout)) {
				if(driverNotification != I2cmError_None) {
					config->pxCurrentTransaction->xTransactionSuccess = pdFALSE;
				} else {
					config->pxCurrentTransaction->xTransactionSuccess = pdTRUE;
				}
			} else {
				config->pxCurrentTransaction->xTransactionSuccess = pdFALSE;
			}
			xTaskNotify(config->pxCurrentTransaction->xRequestingTask, 0, eNoAction);
		}
	}
}

void vI2cInterruptInit(I2CInterruptConfig_t* config) {
	xTaskCreate(vI2cInterruptTask, "I2C", I2C_STACK_SIZE, config, config->uxPriority, NULL);
	
	config->xTransactionQueue = xQueueCreate(config->uxQueueLength, sizeof(I2CTransaction_t*));
}

void vI2CInterruptDisable(I2CInterruptConfig_t* config) {
	Sercom* sercomdevice = (Sercom*)config->sercom;
	
	sercomdevice->I2CM.CTRLA.bit.ENABLE = 0;
}

void vI2CInterruptEnable(I2CInterruptConfig_t* config) {
	Sercom* sercomdevice = (Sercom*)config->sercom;
	
	sercomdevice->I2CM.CTRLA.bit.ENABLE = 1;
}

void vI2cInterruptMBRoutine(I2CInterruptConfig_t* config) {
	Sercom* sercomdevice = (Sercom*)config->sercom;
	
	sercomdevice->I2CM.INTFLAG.bit.MB = 1;
	
	if(sercomdevice->I2CM.STATUS.bit.BUSERR) {
		xTaskNotifyFromISR(config->xI2cTask, I2cmError_BusError, eSetValueWithOverwrite, NULL);
		sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckStop;
		return;
	}
	if(sercomdevice->I2CM.STATUS.bit.ARBLOST) {
		xTaskNotifyFromISR(config->xI2cTask, I2cmError_ArbLost, eSetValueWithOverwrite, NULL);
		sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckStop;
		return;
	}
	if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
		xTaskNotifyFromISR(config->xI2cTask, I2cmError_NoSlave, eSetValueWithOverwrite, NULL);
		sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckStop;
		return;
	}

	switch(config->xState) {
		case I2C_WRITE_REGISTER:
			sercomdevice->I2CM.DATA.bit.DATA = config->pxCurrentTransaction->ucRegisterAddress;
			config->pxCurrentTransaction->ucDataCounter = 0;
			if(config->pxCurrentTransaction->xTransactionType == I2C_WRITE) {
				config->xState = I2C_WRITE_DATA;
			} else if (config->pxCurrentTransaction->xTransactionType == I2C_WRITEREAD) {
				config->xState = I2C_READ_DATA;
			}
			break;
		case I2C_READ_DATA:
			sercomdevice->I2CM.ADDR.reg = config->pxCurrentTransaction->ucDeviceAddress | I2C_READMASK;
			break;
		case I2C_WRITE_DATA:
			if(config->pxCurrentTransaction->ucDataCounter < config->pxCurrentTransaction->ucDataLength) {
				sercomdevice->I2CM.DATA.bit.DATA = config->pxCurrentTransaction->pcData[config->pxCurrentTransaction->ucDataCounter++];
			} else {
				xTaskNotifyFromISR(config->xI2cTask, I2cmError_None, eSetValueWithOverwrite, NULL);
			}
			break;
		default:
			sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckStop;
			break;
	}
}

void vI2cInterruptSBRoutine(I2CInterruptConfig_t* config) {
	Sercom* sercomdevice = (Sercom*)config->sercom;
	
	sercomdevice->I2CM.INTFLAG.bit.SB = 1;
	
	if(config->pxCurrentTransaction->xBigEndian == pdTRUE) {
		config->pxCurrentTransaction->ucDataCounter++;
		config->pxCurrentTransaction->pcData[config->pxCurrentTransaction->ucDataLength - (config->pxCurrentTransaction->ucDataCounter)] = (uint8_t)sercomdevice->I2CM.DATA.bit.DATA;
	} else {
		config->pxCurrentTransaction->pcData[config->pxCurrentTransaction->ucDataCounter++] = (uint8_t)sercomdevice->I2CM.DATA.bit.DATA;
	}
	
	if(config->pxCurrentTransaction->ucDataCounter < config->pxCurrentTransaction->ucDataLength) {
		sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
		sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckRead;
	} else {
		sercomdevice->I2CM.CTRLB.bit.ACKACT = 1;
		sercomdevice->I2CM.CTRLB.bit.CMD = I2cmCmd_AckStop;
		xTaskNotifyFromISR(config->xI2cTask, I2cmError_None, eSetBits, NULL);
	}
}

void vI2cInterruptErrorRoutine(I2CInterruptConfig_t* config) {
	Sercom* sercomdevice = (Sercom*)config->sercom;
	sercomdevice->I2CM.STATUS.bit.BUSSTATE = 0x01;
	sercomdevice->I2CM.INTFLAG.bit.ERROR = 1;
	xTaskNotifyFromISR(config->xI2cTask, I2cmError_BusError, eSetBits, NULL);
}