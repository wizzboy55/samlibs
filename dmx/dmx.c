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
#include "rdm.h"

#include <string.h>

#define DMX_BAUDRATE 250000

DmxBuffer_t* pDmxPortGetNextBuffer(DmxPortConfig_t* config) {
	for(uint8_t i = 0; i < DMX_BUFFERING; i++) {
		if(config->rxBuffers[i].status.written == 0 && config->rxBuffers[i].status.read == 0 && config->rxBuffers[i].status.valid == 0) {
			DmxBuffer_t* nextBuffer = &(config->rxBuffers[i]);
			nextBuffer->status.written = 1;
			nextBuffer->slotCount = 0;
			return nextBuffer;
		}
	}
	// No buffer was found, lower criteria
	for(uint8_t i = 0; i < DMX_BUFFERING; i++) {
		if(config->rxBuffers[i].status.written == 0 && config->rxBuffers[i].status.read == 0) {
			DmxBuffer_t* nextBuffer = &(config->rxBuffers[i]);
			nextBuffer->status.written = 1;
			nextBuffer->slotCount = 0;
			return nextBuffer;
		}
	}
	
	return NULL;
}

void vDmxSetupPins(Sercom* sercomdevice, uint32_t rxpin, uint32_t txpin, uint8_t pinmux, uint32_t dirpin) {
	if(sercomdevice->USART.CTRLA.bit.ENABLE) {
		return;	
	}
	
	if(pinmux == 0) {
		return;
	}
	
	if(rxpin != GPIO_NONE) {
		int8_t rxbit = xSercomPadToRXPO(eSercomGPIOToPad(rxpin));
		if(rxbit >= 0) {
			sercomdevice->USART.CTRLA.bit.RXPO = (uint8_t)rxbit;
			samgpio_setPinFunction(rxpin, pinmux);
			sercomdevice->USART.CTRLB.bit.RXEN = 1;
		}
	}
	
	if(txpin != GPIO_NONE) {
		int8_t txbit = xSercomPadToTXPO(eSercomGPIOToPad(txpin));
		if(txbit >= 0) {
			sercomdevice->USART.CTRLA.bit.TXPO = (uint8_t)txbit;
			samgpio_setPinFunction(txpin, pinmux);
			sercomdevice->USART.CTRLB.bit.TXEN = 1;
		}
	}
	
	if(dirpin != GPIO_NONE) {
		samgpio_setPinLevel(dirpin, 0);
		samgpio_setPinDirection(dirpin, GPIO_DIRECTION_OUT);
	}
}

void vDmxTask(void* p) {
	DmxPortConfig_t* config = (DmxPortConfig_t*)p;
	
#define TICK_BLINK_DMX 90 / portTICK_PERIOD_MS
#define TICK_BLINK_RDM 90 / portTICK_PERIOD_MS
	
	TickType_t blink_delay = TICK_BLINK_DMX;
	
	for(;;) {
		vTaskDelay(blink_delay);
		if(config->activity == DmxActivity_Dmx) {
			samgpio_togglePinLevel(config->hw.ledpin);
			blink_delay = TICK_BLINK_DMX;
		} else if(config->activity == DmxActivity_Rdm) {
			samgpio_togglePinLevel(config->hw.ledpin);
			blink_delay = TICK_BLINK_RDM;
		} else {
			samgpio_setPinLevel(config->hw.ledpin, 1 ^ config->hw.ledinv);
			blink_delay = TICK_BLINK_DMX;
		}
		
		config->activity = DmxActivity_Idle;
	}
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
	
	sercomdevice->USART.CTRLA.bit.SAMPR = 0;
	sercomdevice->USART.CTRLA.bit.SAMPA = 0x03;
	sercomdevice->USART.CTRLA.bit.MODE = 0x01;
	
	sercomdevice->USART.CTRLB.bit.PMODE = 0;	// No Parity
	sercomdevice->USART.CTRLB.bit.SBMODE = 0;	// One Stop Bit, to prevent framing error with devices sending short stop bits;
	sercomdevice->USART.CTRLB.bit.CHSIZE = 0;	// 8-bit Data
	
	vDmxSetupPins(sercomdevice, config->hw.rxpin, config->hw.txpin, config->hw.pinmux, config->hw.dirpin);
	
	samgpio_setPinDirection(config->hw.ledpin, GPIO_DIRECTION_OUT);
	samgpio_setPinLevel(config->hw.ledpin, 1 ^ config->hw.ledinv);
	
	sercomdevice->USART.INTFLAG.reg = 0x00;
	sercomdevice->USART.INTENCLR.reg = 0xFF;
	sercomdevice->USART.INTENSET.bit.RXC = 1;
	
	config->rxState = DmxState_Idle;
	config->txState = DmxState_Idle;
	
	memset(config->rxBuffers, 0, sizeof(config->rxBuffers));
	
	config->currentRxBuffer = pDmxPortGetNextBuffer(config);
	config->currentTxBuffer = NULL;
	
	config->lastValidRxBuffer = NULL;
	
	xTaskCreate(vDmxTask, "dmx", configMINIMAL_STACK_SIZE, (void*)config, configMAX_PRIORITIES - 3, NULL);
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 1;
	
	sam_enableModuleInterrupt(sercomdevice);
	
	return pdPASS;
}

inline void vDmxPortPushNewFrame(DmxPortConfig_t* config) {
	config->currentRxBuffer->status.written = 0;
	config->currentRxBuffer->status.valid = 1;
	config->lastValidRxBuffer = config->currentRxBuffer;
	config->currentRxBuffer = pDmxPortGetNextBuffer(config);
	if(config->cb_newRxFrame != NULL) {
		config->cb_newRxFrame(config->lastValidRxBuffer);
	}
}

inline void vDmxPortPushNewRdmMessage(DmxPortConfig_t* config) {
	config->currentRxBuffer->status.written = 0;
	config->currentRxBuffer->status.valid = 1;
	config->lastValidRxBuffer = config->currentRxBuffer;
	config->currentRxBuffer = pDmxPortGetNextBuffer(config);
	if(config->cb_newRdmMessage != NULL) {
		config->cb_newRdmMessage(config->lastValidRxBuffer);
	}
}

void vDmxEndTransmission(DmxPortConfig_t* config) {
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	sercomdevice->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE | SERCOM_USART_INTENCLR_TXC;
	config->txState = DmxState_Idle;
	if(config->currentTxBuffer != NULL) {
		config->currentTxBuffer->status.valid = 0;
		config->currentTxBuffer->status.read = 0;
	}
}

inline void vDmxInterruptHandler(DmxPortConfig_t* config) {
	if(config->hw.module == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	if(sercomdevice->USART.INTFLAG.bit.RXC) {
		// RX Complete Interrupt
		uint8_t data_byte = sercomdevice->USART.DATA.reg;
		
		if(config->cb_byteReceived != NULL) {
			config->cb_byteReceived();
		}
		
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
						config->currentRxBuffer->dmx[DMX_STARTCODE_INDEX] = data_byte;
					}
					config->currentRxBuffer->slotCount = 1;
					if(data_byte == DMX_STARTCODE) {
						config->rxState = DmxState_Slots;
					} else if(RDM_STARTCODE) {
						config->rxState = DmxState_Rdm;
					} else {
						config->rxState = DmxState_Idle;
					}
					break;
				case DmxState_Slots:
					if(config->currentRxBuffer != NULL) {
						config->currentRxBuffer->dmx[config->currentRxBuffer->slotCount++] = data_byte;
						if(config->currentRxBuffer->slotCount > DMX_MAX_SLOTS) {
							vDmxPortPushNewFrame(config);
							config->activity = DmxActivity_Dmx;
							config->rxState = DmxState_Idle;
						}
					}
					break;
				case DmxState_Rdm:
					if(config->currentRxBuffer != NULL) {
						config->currentRxBuffer->dmx[config->currentRxBuffer->slotCount++] = data_byte;
						if(config->currentRxBuffer->slotCount >= config->currentRxBuffer->rdm.messageLength + sizeof(RdmChecksum_t) && config->currentRxBuffer->slotCount >= RDM_MINIMUMFRAMELENGTH) {
							vDmxPortPushNewRdmMessage(config);
							config->activity = DmxActivity_Rdm;
							config->rxState = DmxState_Idle;
						}
					}
					break;
				default:
					config->rxState = DmxState_Idle;
			}	
		}
	}
	
	if(sercomdevice->USART.INTFLAG.bit.TXC || sercomdevice->USART.INTFLAG.bit.DRE) {
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
				samgpio_setPinFunction(config->hw.txpin, config->hw.pinmux);
				config->currentTxSlot = 1;
				sercomdevice->USART.DATA.reg = config->currentTxBuffer->dmx[DMX_STARTCODE_INDEX];
				config->txState = DmxState_Slots;
				break;
			case DmxState_Slots:
				sercomdevice->USART.DATA.reg = config->currentTxBuffer->dmx[config->currentTxSlot];
				if(config->currentTxSlot >= config->currentTxBuffer->slotCount) {
					vDmxEndTransmission(config);
				} else {
					config->currentTxSlot++;
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
	
	
	frame->status.read = 1;
	// Generate break
	config->txState = DmxState_Break;
	samgpio_clearPin(config->hw.txpin);
	samgpio_setPinDirection(config->hw.txpin, GPIO_DIRECTION_OUT);
	samgpio_setPinFunction(config->hw.txpin, GPIO_PIN_FUNCTION_OFF);
	config->currentTxSlot = 1;
	config->currentTxBuffer = frame;
	sercomdevice->USART.DATA.reg = DMX_BREAKBYTE;
	sercomdevice->USART.INTENSET.bit.TXC = 1;
	
	return pdPASS;
}

void vDmxSwapRxTxPins(DmxPortConfig_t* config, BaseType_t swap) {
	if(config->hw.module == NULL) {
		return;
	}
	
	Sercom* sercomdevice = (Sercom *)config->hw.module;
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 0;
	
	sercomdevice->USART.CTRLB.bit.RXEN = 0;
	sercomdevice->USART.CTRLB.bit.TXEN = 0;
	
	if(swap == pdTRUE) {
		vDmxSetupPins(sercomdevice, config->hw.txpin, config->hw.txpin, config->hw.pinmux, config->hw.dirpin);
	} else {
		vDmxSetupPins(sercomdevice, config->hw.rxpin, config->hw.rxpin, config->hw.pinmux, config->hw.dirpin);
	}
	
	sercomdevice->USART.CTRLA.bit.ENABLE = 1;
}

DmxBuffer_t* pDmxGetLastValidFrame(DmxPortConfig_t* config) {
	return config->lastValidRxBuffer;
}

void vDmxCopyFrame(DmxBuffer_t* dest, DmxBuffer_t* source) {
	if(dest == NULL || source == NULL) {
		return;
	}
	dest->status.written = 1;
	memcpy(dest->dmx, source->dmx, source->slotCount);
	dest->slotCount = source->slotCount;
	dest->status.valid = 1;
	dest->status.written = 0;
}