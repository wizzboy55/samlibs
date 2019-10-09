/*
 * dmx.c
 *
 * Created: 2019-09-20 5:14:29 PM
 *  Author: charl
 */ 

#include "dmx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "samclk.h"
#include "samgpio.h"
#include "saminterrupt.h"
#include "utilities.h"

#include <string.h>

#define DMX_BAUDRATE 250000

#define DMXDEBUGPIN GPIO(GPIO_PORTA, 25)

DmxBuffer_t* pDmxPortGetNextBuffer(DmxPortConfig_t* config) {
	for(uint8_t i = 0; i < DMX_BUFFERING; i++) {
		if(config->rxBuffers[i].status.used == 0 && config->rxBuffers[i].status.valid == 0) {
			DmxBuffer_t* nextBuffer = &(config->rxBuffers[i]);
			nextBuffer->status.used = 1;
			nextBuffer->slotCount = 0;
			return nextBuffer;
		}
	}
	
	return NULL;
}

BaseType_t xDmxInitSercom(DmxPortConfig_t* config) {
	if(config->hw.module == NULL) {
		return pdFAIL;
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	samclk_enable_peripheral_clock(config->hw.module);
	samclk_enable_gclk_channel(config->hw.module, 0);
	
	sercomdevice->USART.CTRLA.bit.SWRST = 1;
	
	while(sercomdevice->USART.CTRLA.bit.SWRST) {}
	
	sercomdevice->USART.BAUD.reg = 65536 - ((65536 * 16.0f * DMX_BAUDRATE) / CONF_CPU_FREQUENCY);
	
	sercomdevice->USART.CTRLA.bit.DORD = 1;
	sercomdevice->USART.CTRLA.bit.FORM = 0;
	sercomdevice->USART.CTRLA.bit.RXPO = xSercomPadToRXPO(config->hw.rxpad);
	sercomdevice->USART.CTRLA.bit.TXPO = xSercomPadToTXPO(config->hw.txpad);
	sercomdevice->USART.CTRLA.bit.SAMPR = 0;
	sercomdevice->USART.CTRLA.bit.SAMPA = 0x03;
	sercomdevice->USART.CTRLA.bit.MODE = 0x01;
	
	sercomdevice->USART.CTRLB.bit.PMODE = 0;	// No Parity
	sercomdevice->USART.CTRLB.bit.SBMODE = 0;	// One Stop Bit, to prevent framing error with devices sending short stop bits;
	sercomdevice->USART.CTRLB.bit.CHSIZE = 0;	// 8-bit Data
	
	if(config->hw.rxfunc != 0) {
		samgpio_setPinFunction(config->hw.rxpin, config->hw.rxfunc);
		sercomdevice->USART.CTRLB.bit.RXEN = 1;
	}
	
	if(config->hw.txfunc != 0) {
		samgpio_setPinDirection(config->hw.txpin, GPIO_DIRECTION_OUT);
		samgpio_setPinLevel(config->hw.txpin, 0);
		samgpio_setPinFunction(config->hw.txpin, config->hw.txfunc);
		sercomdevice->USART.CTRLB.bit.TXEN = 1;
	}
	
	sercomdevice->USART.INTFLAG.reg = 0x00;
	sercomdevice->USART.INTENCLR.reg = 0xFF;
	sercomdevice->USART.INTENSET.bit.RXC = 1;
	
	config->rxState = DmxState_Idle;
	config->txState = DmxState_Idle;
	
	memset(config->rxBuffers, 0, sizeof(config->rxBuffers));
	
	config->currentRxBuffer = pDmxPortGetNextBuffer(config);
	config->currentTxBuffer = NULL;
	
	config->lastValidRxBuffer = NULL;
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 1;
	
	sam_enableModuleInterrupt(sercomdevice);
	
	samgpio_setPinDirection(DMXDEBUGPIN, GPIO_DIRECTION_OUT);
	
	return pdPASS;
}

inline void vDmxPortPushNewFrame(DmxPortConfig_t* config) {
	config->currentRxBuffer->status.used = 0;
	config->currentRxBuffer->status.valid = 1;
	config->lastValidRxBuffer = config->currentRxBuffer;
	config->currentRxBuffer = pDmxPortGetNextBuffer(config);
	config->cb_newRxFrame(config->lastValidRxBuffer);
	samgpio_togglePinLevel(DMXDEBUGPIN);
}

void vDmxEndTransmission(DmxPortConfig_t* config) {
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	sercomdevice->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE | SERCOM_USART_INTENCLR_TXC;
	config->txState = DmxState_Idle;
}

inline void vDmxInterruptHandler(DmxPortConfig_t* config) {
	if(config->hw.module == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	if(sercomdevice->USART.INTFLAG.bit.RXC) {
		// RX Complete Interrupt
		uint8_t data_byte = sercomdevice->USART.DATA.reg;
		
		if(sercomdevice->USART.STATUS.bit.BUFOVF) {
			// Overflow
			// Reset port since data is invalid
			sercomdevice->USART.STATUS.bit.BUFOVF = 1;
		} else if(sercomdevice->USART.STATUS.bit.FERR) {
			// Framing Error = Break Condition
			sercomdevice->USART.STATUS.bit.FERR = 1;
			if(config->currentRxBuffer->slotCount != 0) {
				// Incomplete frame received
				vDmxPortPushNewFrame(config);
			}
			config->rxState = DmxState_StartCode;
		} else {
			switch(config->rxState) {
				case DmxState_Idle:
					// Loose or extra byte
					UNUSED(data_byte);
					break;
				case DmxState_StartCode:
					if(config->currentRxBuffer != NULL) {
						config->currentRxBuffer->dmx.startcode = data_byte;
					}
					config->currentRxBuffer->slotCount = 0;
					if(data_byte == DMX_STARTCODE_DMX) {
						config->rxState = DmxState_Slots;
					} else if(DMX_STARTCODE_RDM) {
						config->rxState = DmxState_Rdm;
					} else {
						config->rxState = DmxState_Idle;
					}
					break;
				case DmxState_Slots:
					if(config->currentRxBuffer != NULL) {
						config->currentRxBuffer->dmx.slots[config->currentRxBuffer->slotCount++] = data_byte;
						if(config->currentRxBuffer->slotCount >= DMX_MAX_SLOTS) {
							vDmxPortPushNewFrame(config);
							config->rxState = DmxState_Idle;
						}
					}
					break;
				case DmxState_Rdm:
			
					break;
				default:
					config->rxState = DmxState_Idle;
			}	
		}
	}
	
	if(sercomdevice->USART.INTFLAG.bit.TXC) {
		// Data TX Complete Interrupt
		sercomdevice->USART.INTFLAG.bit.TXC = 1;
		switch(config->txState) {
			case DmxState_Break:
				sercomdevice->USART.DATA.reg = DMX_BREAKBYTE;
				config->currentTxSlot++;
				if(config->currentTxSlot >= config->breakByteCount) {
					config->txState = DmxState_Mark;
				}
				break;
			case DmxState_Mark:
				samgpio_setPin(config->hw.txpin);
				sercomdevice->USART.DATA.reg = DMX_BREAKBYTE;
				config->txState = DmxState_StartCode;
				break;
			case DmxState_StartCode:
				samgpio_setPinFunction(config->hw.txpin, config->hw.txfunc);
				config->currentTxSlot = 0;
				sercomdevice->USART.DATA.reg = config->currentTxBuffer->dmx.startcode;
				config->txState = DmxState_Slots;
				break;
			case DmxState_Slots:
				sercomdevice->USART.DATA.reg = config->currentTxBuffer->dmx.slots[config->currentTxSlot++];
				if(config->currentTxSlot >= config->currentTxBuffer->slotCount) {
					vDmxEndTransmission(config);
				}
				break;
			default:
				vDmxEndTransmission(config);
		}
	}
}

BaseType_t xDmxSendFrame(DmxPortConfig_t* config, DmxBuffer_t* frame) {
	if(config == NULL || config->hw.module == NULL || frame == NULL) {
		return pdFAIL;
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	if(config->txState != DmxState_Idle) {
		return pdFAIL;
	}
	
	// Generate break
	config->txState = DmxState_Break;
	samgpio_clearPin(config->hw.txpin);
	samgpio_setPinFunction(config->hw.txpin, GPIO_PIN_FUNCTION_OFF);
	config->currentTxSlot = 1;
	config->currentTxBuffer = frame;
	sercomdevice->USART.DATA.reg = DMX_BREAKBYTE;
	sercomdevice->USART.INTENSET.bit.TXC = 1;
	
	return pdPASS;
}