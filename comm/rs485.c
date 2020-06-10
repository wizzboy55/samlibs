/*
 * rs485.c
 *
 * Created: 2020-05-14 4:37:13 PM
 *  Author: charl
 */ 

#include "rs485.h"
#include "samclk.h"
#include "saminterrupt.h"

BaseType_t xRs485Init(Rs485Config_t* config) {
	if(config == NULL) {
		return pdFAIL;
	}
	
	if(config->module == NULL) {
		return pdFAIL;
	}
	
	Sercom* sercomdevice = (Sercom*)config->module;
	
	samclk_enable_peripheral_clock(config->module);
	samclk_enable_gclk_channel(config->module, 0);
	
	if(config->txpin == GPIO_NONE || config->rxpin == GPIO_NONE || config->dirpin == GPIO_NONE) {
		return pdFAIL;
	}
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 0;
	
	if(eSercomGPIOToPad(config->txpin) != PAD0 || eSercomGPIOToPad(config->dirpin) != PAD2) {
		return pdFAIL;
	}
		
	sercomdevice->USART.CTRLA.bit.TXPO = 0x03;	// Fixed value to use PAD2 as DIR
	samgpio_setPinFunction(config->txpin, config->pinmux);
	samgpio_setPinPullMode(config->txpin, GPIO_PULL_UP);
	samgpio_setPinFunction(config->dirpin, config->pinmux);
	
	int8_t rxbit = xSercomPadToRXPO(eSercomGPIOToPad(config->rxpin));
	if(rxbit >= 0) {
		sercomdevice->USART.CTRLA.bit.RXPO = (uint8_t)rxbit;
		samgpio_setPinFunction(config->rxpin, config->pinmux);
		samgpio_setPinPullMode(config->rxpin, GPIO_PULL_UP);
		sercomdevice->USART.CTRLB.bit.RXEN = 1;
		sercomdevice->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;
	} else {
		return pdFAIL;
	}
	
	samgpio_setPinDirection(config->ledpin, GPIO_DIRECTION_OUT);
	
	sercomdevice->USART.BAUD.reg = ulSercomBaudrateToRegisterArithmetic(config->baudrate);
	
	sercomdevice->USART.CTRLA.bit.DORD = uSercomDataOrderToRegister(config->dataOrder);
	
	sercomdevice->USART.CTRLA.bit.CMODE = 0; // Async Mode
	
	sercomdevice->USART.CTRLA.bit.IBON = 1;
	
	sercomdevice->USART.CTRLA.bit.FORM = uSercomParityToFrameFormat(config->parity);
	
	sercomdevice->USART.CTRLA.bit.MODE = 0x01;
	
	sercomdevice->USART.CTRLB.bit.PMODE = uSercomParityToRegister(config->parity);
	
	sercomdevice->USART.CTRLB.bit.SBMODE = config->stopBits;
	
	sercomdevice->USART.CTRLB.bit.CHSIZE = config->characterSize;
	
	sercomdevice->USART.CTRLC.bit.GTIME = config->guardTime;
	
	if(config->useInterrupt) {
		sam_enableModuleInterrupt(config->module);
	}
	
	while(sercomdevice->USART.INTFLAG.bit.RXC) {
		uint16_t dump = sercomdevice->USART.DATA.reg;
		dump++;
	}
	
	sercomdevice->USART.INTENSET.reg = SERCOM_USART_INTENSET_ERROR;
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 1;
	
	return pdPASS;
}

void vRs485SendChar(Rs485Config_t* config, uint16_t c) {
	if(config == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom*)config->module;
	
	sercomdevice->USART.DATA.reg = c;
}

void vRs485SendCharWait(Rs485Config_t* config, uint16_t c) {
	if(config == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom*)config->module;
	
	sercomdevice->USART.DATA.reg = c;
	
	while(sercomdevice->USART.INTFLAG.bit.TXC == 0) {
		
	}
}

void vRs485SendBuffer(Rs485Config_t* config, uint8_t* buffer, Rs485Size_t size) {
	if(config == NULL || buffer == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom*)config->module;
	
	for(Rs485Size_t i = 0; i < size; i++) {
		sercomdevice->USART.DATA.reg = buffer[i];
		while(sercomdevice->USART.INTFLAG.bit.DRE == 0) {
			
		}
	}
}

BaseType_t xRs485ReceiveChar(Rs485Config_t* config, uint16_t* c) {
	if(config == NULL) {
		return pdFAIL;
	}
	
	Sercom* sercomdevice = (Sercom*)config->module;
	
	uint16_t cnt = 5000;
	
	while(cnt--) {
		if(sercomdevice->USART.INTFLAG.bit.RXC == 1) {
			*c = sercomdevice->USART.DATA.reg;
			
			return pdPASS;
		}
	}
	
	return pdFAIL;
}

void vRs485SetLinkLed(Rs485Config_t* config) {
	if(config == NULL) {
		return;
	}
	
	if(config->ledinv) {
		samgpio_clearPinFast(config->ledpin);
	} else {
		samgpio_setPinFast(config->ledpin);
	}
}

void vRs485ClearLinkLed(Rs485Config_t* config) {
	if(config == NULL) {
		return;
	}
	
	if(config->ledinv) {
		samgpio_setPinFast(config->ledpin);
	} else {
		samgpio_clearPinFast(config->ledpin);
	}
}

BaseType_t xRs485SendMessage(Rs485Status_t* status, uint8_t* message, Rs485Size_t size, TickType_t timeout) {
	if(status == NULL) {
		return pdFAIL;
	}
	
	if(status->config == NULL) {
		return pdFAIL;
	}
	
	Sercom* sercomdevice = (Sercom*)status->config->module;
	
	status->txBuffer = message;
	status->txBufferSize = size;
	status->txBufferIndex = 1;
	
	sercomdevice->USART.CTRLB.bit.TXEN = 1;
	samgpio_setPinFunction(status->config->rxpin, GPIO_PIN_FUNCTION_OFF); // Prevent spurious reception when no external pull-up is present
	
	if(status->config->characterSize == Char9bit) {
		sercomdevice->USART.DATA.reg = RS485_ADDRESSMASK | message[0];
	} else {
		sercomdevice->USART.DATA.reg = message[0];
	}
	sercomdevice->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;
	sercomdevice->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
	BaseType_t res = xTaskNotifyWait(0, 0, NULL, timeout);
	
	sercomdevice->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_TXC || SERCOM_USART_INTENCLR_DRE;
	
	return res;
}

inline void vRs485InterruptHandler(Rs485Status_t* status) {
	if(status == NULL) {
		return;
	}
	
	if(status->config == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom*)status->config->module;
	
	while(sercomdevice->USART.INTFLAG.bit.ERROR) {
		sercomdevice->USART.STATUS.reg = 0xFF;
		sercomdevice->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_ERROR;
	}
	
	while(sercomdevice->USART.INTFLAG.bit.RXC) {
		
		uint16_t temp = sercomdevice->USART.DATA.reg;
		if(temp & RS485_ADDRESSMASK) {
			status->rxBufferIndex = 0;
		}
		if(status->rxBufferIndex < status->rxBufferSize && status->rxBuffer != NULL) {
			status->rxBuffer[status->rxBufferIndex++] = temp & 0xFF;
		}
		if(status->rxBufferIndex == status->rxBufferSize) {
			if(status->rxQueue != NULL) {
				xQueueSendToBackFromISR(status->rxQueue, status->rxBuffer, NULL);
			}
			status->rxBufferIndex++;
		}
	}
	
	if(sercomdevice->USART.INTFLAG.bit.DRE && sercomdevice->USART.INTENSET.bit.DRE) {
		if((status->txBufferIndex < status->txBufferSize) && (status->txBuffer != NULL)) {
			sercomdevice->USART.DATA.reg = status->txBuffer[status->txBufferIndex++];
		} 
		if(status->txBufferIndex >= status->txBufferSize) {
			sercomdevice->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
			sercomdevice->USART.INTENSET.reg = SERCOM_USART_INTENSET_TXC;
		}
	}
	
	if(sercomdevice->USART.INTFLAG.bit.TXC && sercomdevice->USART.INTENSET.bit.TXC) {
		sercomdevice->USART.INTFLAG.bit.TXC = 1;
		if(status->txBufferIndex >= status->txBufferSize) {
			sercomdevice->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_TXC || SERCOM_USART_INTENCLR_DRE;
			sercomdevice->USART.CTRLB.bit.TXEN = 0;
			samgpio_setPinFunction(status->config->rxpin, status->config->pinmux); // Prevent spurious reception when no external pull-up is present
			vTaskNotifyGiveFromISR(status->txTask, NULL);
		}
	}
}