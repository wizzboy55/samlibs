/*
 * spi_shiftregisters.c
 *
 * Created: 2020-03-18 2:26:59 PM
 *  Author: charl
 */ 

#include "spi_shiftregisters.h"
#include "samclk.h"
#include "saminterrupt.h"

BaseType_t xSpiRegistersInitHardware(SpiShiftRegistersConfig_t* config) {
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	samclk_enable_gclk_channel(config->hw.module, 0);
	samclk_enable_peripheral_clock(config->hw.module);
	
	samgpio_setPinFunction(config->hw.pin_mosi, config->hw.pinmux_sercom);
	samgpio_setPinFunction(config->hw.pin_miso, config->hw.pinmux_sercom);
	samgpio_setPinFunction(config->hw.pin_clk, config->hw.pinmux_sercom);
	
	samgpio_setPinDirection(config->hw.pin_cs, GPIO_DIRECTION_OUT);
	samgpio_setPinLevel(config->hw.pin_cs, 1);
	
	// TODO : Trap invalid configurations and return pdFAIL
	sercomdevice->SPI.CTRLA.bit.DOPO = xSercomPadToDOPO(eSercomGPIOToPad(config->hw.pin_mosi), eSercomGPIOToPad(config->hw.pin_clk));
	sercomdevice->SPI.CTRLA.bit.DIPO = xSercomPadToDIPO(eSercomGPIOToPad(config->hw.pin_miso));
	
	sercomdevice->SPI.CTRLA.bit.MODE = 0x03;
	sercomdevice->SPI.CTRLA.bit.FORM = 0x00;
	sercomdevice->SPI.CTRLB.bit.CHSIZE = 0x00;
	sercomdevice->SPI.CTRLA.bit.DORD = config->dataOrder;
	sercomdevice->SPI.CTRLB.bit.MSSEN = 0;
	sercomdevice->SPI.BAUD.reg = uSercomBaudrateToRegisterSynchronous(config->hw.baud);
	sercomdevice->SPI.CTRLA.bit.CPHA = 0;
	sercomdevice->SPI.CTRLA.bit.CPOL = 0;
	sercomdevice->SPI.CTRLB.bit.RXEN = 1;
	
	sercomdevice->SPI.CTRLA.bit.ENABLE = 1;
	
	config->currentReadMessage = NULL;
	config->currentWriteMessage = NULL;
	config->currentReadIndex = 0;
	config->currentWriteIndex = 0;
	
	config->isInit = pdTRUE;
	
	return pdPASS;
}

void vSpiRegistersReadWriteRegisters(SpiShiftRegistersConfig_t* config, uint8_t* write_message, uint8_t* read_message, uint32_t message_size) {
	if(config == NULL) {
		return;
	}
	
	if(config->isInit == pdFALSE) {
		if(xSpiRegistersInitHardware(config) == pdFAIL) {
			return;
		}
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	config->currentWriteMessage = write_message;
	config->currentReadMessage = read_message;
	config->currentMessageSize = message_size;
	config->currentReadIndex = 0;
	config->currentWriteIndex = 0;
	
	samgpio_setPinLevel(GPIO(GPIO_PORTA, 15), 0);
	samgpio_setPinLevel(config->hw.pin_cs, 0);
	samgpio_setPinLevel(config->hw.pin_cs, 1);
	samgpio_setPinLevel(GPIO(GPIO_PORTA, 15), 1);

	if(config->currentWriteMessage == NULL) {
		sercomdevice->SPI.DATA.reg = 0x00;	
		config->currentWriteIndex++;
	} else {
		sercomdevice->SPI.DATA.reg = config->currentWriteMessage[config->currentWriteIndex++];
	}
	
	do {
		while(sercomdevice->SPI.INTFLAG.bit.TXC == 0) {
			
		}
		
		if(config->currentReadMessage != NULL) {
			config->currentReadMessage[config->currentReadIndex++] = sercomdevice->SPI.DATA.reg;
		} else {
			uint8_t dump = sercomdevice->SPI.DATA.reg;
			config->currentReadIndex++;
			dump++;
		}
		
		if(config->currentWriteMessage == NULL) {
			sercomdevice->SPI.DATA.reg = 0x00;
			config->currentWriteIndex++;
		} else {
			sercomdevice->SPI.DATA.reg = config->currentWriteMessage[config->currentWriteIndex++];
		}
	

	} while(config->currentWriteIndex < config->currentMessageSize);
	
	while(sercomdevice->SPI.INTFLAG.bit.TXC == 0) {
			
	}
	
	samgpio_setPinLevel(GPIO(GPIO_PORTA, 15), 0);
	samgpio_setPinLevel(config->hw.pin_cs, 0);
	samgpio_setPinLevel(config->hw.pin_cs, 1);
	samgpio_setPinLevel(GPIO(GPIO_PORTA, 15), 1);
}

void vSpiRegistersReadWriteRegistersAsync(SpiShiftRegistersConfig_t* config, uint8_t* write_message, uint8_t* read_message, uint32_t message_size, TaskHandle_t task) {
	if(config == NULL) {
		return;
	}
	
	if(config->isInit == pdFALSE) {
		if(xSpiRegistersInitHardware(config) == pdFAIL) {
			return;
		}
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	config->currentWriteMessage = write_message;
	config->currentReadMessage = read_message;
	config->currentMessageSize = message_size;
	config->currentReadIndex = 0;
	config->currentWriteIndex = 0;
	config->task = task;
	
	samgpio_setPinLevel(config->hw.pin_cs, config->latchPolarity == LatchHigh);
	samgpio_setPinLevel(config->hw.pin_cs, config->latchPolarity == LatchLow);

	sam_clearPendingModuleInterrupt(config->hw.module);
	sam_enableModuleInterrupt(config->hw.module);

	if(config->currentWriteMessage == NULL) {
		sercomdevice->SPI.DATA.reg = config->currentWriteIndex++;
	} else {
		sercomdevice->SPI.DATA.reg = config->currentWriteMessage[config->currentWriteIndex++];
	}
	
	sercomdevice->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_RXC | SERCOM_SPI_INTENSET_DRE;
	
	sercomdevice->SPI.INTENCLR.reg = SERCOM_SPI_INTENCLR_RXC;
}

void vSpiRegistersSpiInterruptHandler(SpiShiftRegistersConfig_t* config) {
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	while(sercomdevice->SPI.INTFLAG.bit.RXC) {
		if(config->currentReadIndex < config->currentMessageSize) {
			if(config->currentReadMessage != NULL) {
				config->currentReadMessage[config->currentReadIndex++] = sercomdevice->SPI.DATA.reg;
				if(config->currentReadIndex >= config->currentMessageSize && config->task != NULL) {
					vTaskNotifyGiveFromISR(config->task, NULL);
					sercomdevice->SPI.INTENCLR.reg = SERCOM_SPI_INTENCLR_RXC;
				}
			} else {
				uint8_t dump = sercomdevice->SPI.DATA.reg;
				config->currentReadIndex++;
				dump++;
			}
		}
	}
	
	while(sercomdevice->SPI.INTFLAG.bit.DRE && sercomdevice->SPI.INTENSET.bit.DRE) {
		if(config->currentWriteIndex < config->currentMessageSize) {
			if(config->currentWriteMessage == NULL) {
				sercomdevice->SPI.DATA.reg = config->currentWriteIndex++;
			} else {
				sercomdevice->SPI.DATA.reg = config->currentWriteMessage[config->currentWriteIndex++];
			}
		} else {
			sercomdevice->SPI.INTENCLR.reg = SERCOM_SPI_INTENCLR_DRE;
			sercomdevice->SPI.INTENSET.reg = SERCOM_SPI_INTENSET_TXC;
			break;
		}
	}
	
	if(sercomdevice->SPI.INTFLAG.bit.TXC) {
		samgpio_setPinLevel(config->hw.pin_cs, config->latchPolarity == LatchHigh);
		samgpio_setPinLevel(config->hw.pin_cs, config->latchPolarity == LatchLow);
		sercomdevice->SPI.INTFLAG.reg = SERCOM_SPI_INTFLAG_TXC;
		sercomdevice->SPI.INTENCLR.reg = SERCOM_SPI_INTENCLR_TXC;
	}
}