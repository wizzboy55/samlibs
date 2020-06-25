/*
 * spi_shiftregisters.h
 *
 * Created: 2020-03-18 2:27:09 PM
 *  Author: charl
 */ 


#ifndef SPI_SHIFTREGISTERS_H_
#define SPI_SHIFTREGISTERS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sercom.h"

enum SpiShiftRegistersLatchPolarity_e {LatchLow = 0, LatchHigh = 1};

typedef struct {
	SercomSpiHwConfig_t hw;
	enum SpiShiftRegistersLatchPolarity_e latchPolarity;
	enum SercomDataOrder_e dataOrder;
	uint8_t isInit;
	uint8_t* currentWriteMessage;
	uint32_t currentWriteIndex;
	uint32_t currentMessageSize;
	uint8_t* currentReadMessage;
	uint32_t currentReadIndex;
	TaskHandle_t task;
} SpiShiftRegistersConfig_t;

BaseType_t xSpiRegistersInitHardware(SpiShiftRegistersConfig_t* config);
void vSpiRegistersReadWriteRegisters(SpiShiftRegistersConfig_t* config, uint8_t* write_message, uint8_t* read_message, uint32_t message_size);
void vSpiRegistersReadWriteRegistersAsync(SpiShiftRegistersConfig_t* config, uint8_t* write_message, uint8_t* read_message, uint32_t message_size, TaskHandle_t task);
void vSpiRegistersSpiInterruptHandler(SpiShiftRegistersConfig_t* config);

#endif /* SPI_SHIFTREGISTERS_H_ */