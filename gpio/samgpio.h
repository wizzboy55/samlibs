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

void samgpio_setPinFunction(const uint32_t gpio, const uint32_t function);
void samgpio_setPinDirection(const uint32_t gpio, const enum gpio_direction direction);
void samgpio_setPinLevel(const uint32_t gpio, const uint8_t level);
void samgpio_setPin(const uint32_t gpio);
void samgpio_clearPin(const uint32_t gpio);
void samgpio_togglePinLevel(const uint32_t gpio);
void samgpio_setPinPullMode(const uint32_t gpio, const enum gpio_pull_mode pull_mode);

#endif /* SAMGPIO_H_ */