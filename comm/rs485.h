/*
 * rs485.h
 *
 * Created: 2020-05-14 4:37:06 PM
 *  Author: charl
 */ 


#ifndef RS485_H_
#define RS485_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "samgpio.h"
#include "sercom.h"

#define RS485_ADDRESSMASK 0x0100

typedef uint16_t Rs485Size_t;
typedef uint8_t Rs485Address_t;

typedef struct {
	void* module;
	GPIOPin_t rxpin;
	GPIOPin_t txpin;
	GPIOPin_t dirpin;
	PinMux_t pinmux;
	GPIOPin_t ledpin;
	BaseType_t ledinv;
	uint32_t baudrate;
	enum SercomDataOrder_e dataOrder;
	enum SercomParity_e parity;
	enum SercomStopBits_e stopBits;
	enum SercomCharacterSize_e characterSize;
	uint8_t guardTime;
	BaseType_t useInterrupt;
} Rs485Config_t;

typedef struct {
	Rs485Config_t* config;
	uint8_t* txBuffer;
	Rs485Size_t txBufferSize;
	Rs485Size_t txBufferIndex;
	uint8_t* rxBuffer;
	Rs485Size_t rxBufferSize;
	Rs485Size_t rxBufferIndex;
	TaskHandle_t txTask;
	QueueHandle_t rxQueue;
} Rs485Status_t;

BaseType_t xRs485Init(Rs485Config_t* config);
void vRs485SendChar(Rs485Config_t* config, uint16_t c);
void vRs485SendCharWait(Rs485Config_t* config, uint16_t c);
void vRs485SendBuffer(Rs485Config_t* config, uint8_t* buffer, uint16_t size);
BaseType_t xRs485ReceiveChar(Rs485Config_t* config, uint16_t* c);
void vRs485SetLinkLed(Rs485Config_t* config);
void vRs485ClearLinkLed(Rs485Config_t* config);

BaseType_t xRs485SendMessage(Rs485Status_t* status, uint8_t* message, Rs485Size_t size, TickType_t timeout);
void vRs485InterruptHandler(Rs485Status_t* status);

#endif /* RS485_H_ */