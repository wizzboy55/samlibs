/*
 * gpio_shiftregisters.h
 *
 * Created: 2020-05-12 1:47:39 PM
 *  Author: charl
 */ 


#ifndef GPIO_SHIFTREGISTERS_H_
#define GPIO_SHIFTREGISTERS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "sercom.h"
#include "samgpio.h"

typedef struct {
	struct {
		GPIOPin_t gpio_clk;
		GPIOPin_t gpio_mosi;
		GPIOPin_t gpio_miso;
		GPIOPin_t gpio_cs;
	} hw;
	enum SercomDataOrder_e dataOrder;
	uint8_t isInit;
} GpioShiftRegistersConfig_t;

BaseType_t xGpioShiftRegistersInit(GpioShiftRegistersConfig_t* config);
BaseType_t xGpioShiftRegistersPush(GpioShiftRegistersConfig_t* config, const uint8_t* writeMessage, uint8_t* readMessage, const uint8_t messageSize);

#endif /* GPIO_SHIFTREGISTERS_H_ */