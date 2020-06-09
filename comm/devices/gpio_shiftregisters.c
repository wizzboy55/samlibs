/*
 * gpio_shiftregisters.c
 *
 * Created: 2020-05-12 1:47:26 PM
 *  Author: charl
 */ 

#include "gpio_shiftregisters.h"



BaseType_t xGpioShiftRegistersInit(GpioShiftRegistersConfig_t* config) {
	if(config == NULL) {
		return pdFAIL;
	}
	
	samgpio_setPinFunction(config->hw.gpio_clk, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinLevel(config->hw.gpio_clk, 0);
	samgpio_setPinDirection(config->hw.gpio_clk, GPIO_DIRECTION_OUT);
	
	samgpio_setPinFunction(config->hw.gpio_mosi, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinDirection(config->hw.gpio_mosi, GPIO_DIRECTION_OUT);
	
	samgpio_setPinFunction(config->hw.gpio_miso, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinDirection(config->hw.gpio_miso, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(config->hw.gpio_miso, GPIO_PULL_UP);
	
	samgpio_setPinFunction(config->hw.gpio_cs, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinLevel(config->hw.gpio_cs, 1);
	samgpio_setPinDirection(config->hw.gpio_cs, GPIO_DIRECTION_OUT);
	
	config->isInit = pdTRUE;
	
	return pdPASS;
}

#define GPIOSR_CLKUP	samgpio_setPinFast(gpio_clk);
#define GPIOSR_CLKDOWN	samgpio_clearPinFast(gpio_clk);
#define GPIOSR_CSUP		samgpio_setPinFast(gpio_cs);
#define GPIOSR_CSDOWN	samgpio_clearPinFast(gpio_cs);

BaseType_t xGpioShiftRegistersPush(GpioShiftRegistersConfig_t* config, const uint8_t* writeMessage, uint8_t* readMessage, const uint8_t messageSize) {
	if(config == NULL) {
		return pdFAIL;
	}
	
	if(config->isInit == pdFALSE) {
		return pdFAIL;
	}
	
	GPIOPin_t gpio_clk = config->hw.gpio_clk;
	GPIOPin_t gpio_cs = config->hw.gpio_cs;
	GPIOPin_t gpio_miso = config->hw.gpio_miso;
	GPIOPin_t gpio_mosi = config->hw.gpio_mosi;
	
	if(readMessage != NULL) {
		GPIOSR_CSDOWN;
		GPIOSR_CSUP;
	}
	for(uint8_t c = messageSize-1; c < messageSize; c--) {
		uint8_t rxByte = 0;
		uint8_t txByte;
		if(writeMessage != NULL) {
			txByte = writeMessage[c];
		} else {
			txByte = 0xFF;
		}
		
		for(uint8_t b = 8-1; b < 8; b--) {
			rxByte = rxByte << 1;
			samgpio_setPinLevelFast(gpio_mosi, (txByte & 1<<b) != 0);
			rxByte |= samgpio_getPinLevelFast(gpio_miso);
			GPIOSR_CLKUP;
			GPIOSR_CLKDOWN;
		}
		if(readMessage != NULL) {
			readMessage[c] = rxByte;
		}
	}
	
	if(writeMessage != NULL) {
		GPIOSR_CSUP;
		GPIOSR_CSDOWN;
	}
	
	return pdPASS;
}