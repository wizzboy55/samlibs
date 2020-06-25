/*
 * samgpio.h
 *
 * Created: 2019-08-17 4:25:29 PM
 *  Author: Wizz
 */ 


#ifndef SAMGPIO_H_
#define SAMGPIO_H_

#include <stdint.h>
#include "hpl_gpio.h"

typedef uint32_t GPIOPin_t;
typedef uint32_t PinMux_t;

#define GPIO_NONE	0xFFFFFFFF

void samgpio_setPinFunction(const GPIOPin_t gpio, const PinMux_t function);
void samgpio_setPinDirection(const GPIOPin_t gpio, const enum gpio_direction direction);
void samgpio_setPinLevel(const GPIOPin_t gpio, const uint8_t level);
void samgpio_setPinLevelFast(const GPIOPin_t gpio, const uint8_t level);
void samgpio_setPinFast(const GPIOPin_t gpio);
void samgpio_clearPinFast(const GPIOPin_t gpio);
void samgpio_togglePinLevelFast(const GPIOPin_t gpio);
void samgpio_setPinPullMode(const GPIOPin_t gpio, const enum gpio_pull_mode pull_mode);
uint8_t samgpio_getPinLevel(const GPIOPin_t gpio);
uint8_t samgpio_getPinLevelFast(const GPIOPin_t gpio);
PinMux_t samgpio_getModulePinmux(const void* module);

#endif /* SAMGPIO_H_ */