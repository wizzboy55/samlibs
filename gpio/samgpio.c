/*
 * samgpio.c
 *
 * Created: 2019-08-17 4:25:38 PM
 *  Author: Wizz
 */ 

#include "samgpio.h"

#include "debug_interface.h"

void samgpio_setPinFunction(const uint32_t gpio, const uint32_t function) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	if (function == GPIO_PIN_FUNCTION_OFF) {
		PORT->Group[port].PINCFG[pin].bit.PMUXEN = 0;
	} else {
		PORT->Group[port].PINCFG[pin].bit.PMUXEN = 1;
		if (pin & 1) {
			// Odd numbered pin
			PORT->Group[port].PMUX[pin >> 1].bit.PMUXO = function & 0xF;
		} else {
			// Even numbered pin
			PORT->Group[port].PMUX[pin >> 1].bit.PMUXE = function & 0xF;
		}
	}
	CRITICAL_SECTION_LEAVE()
}

void samgpio_setPinDirection(const uint32_t gpio, const enum gpio_direction direction) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	switch (direction) {
		case GPIO_DIRECTION_OFF:
			PORT->Group[port].DIRCLR.reg = (1 << pin);
			PORT->Group[port].PINCFG[pin].bit.INEN = 0;
			break;
		case GPIO_DIRECTION_IN:
			PORT->Group[port].DIRCLR.reg = (1 << pin);
			PORT->Group[port].PINCFG[pin].bit.INEN = 1;
			break;
		case GPIO_DIRECTION_OUT:
			PORT->Group[port].DIRSET.reg = (1 << pin);
			PORT->Group[port].PINCFG[pin].bit.INEN = 0;
			break;
		default:
			DEBUG_printf( ("ERROR: GPIO Direction not supported.\n") );
	}
	CRITICAL_SECTION_LEAVE()
}

void samgpio_setPinLevel(const uint32_t gpio, const uint8_t level) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	if(level) {
		PORT->Group[port].OUTSET.reg = (1<<pin);
	} else {
		PORT->Group[port].OUTCLR.reg = (1<<pin);
	}
	CRITICAL_SECTION_LEAVE()
}

void samgpio_setPin(const uint32_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	PORT->Group[port].OUTSET.reg = (1<<pin);
}

void samgpio_clearPin(const uint32_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	PORT->Group[port].OUTCLR.reg = (1<<pin);
}

void samgpio_togglePinLevel(const uint32_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	PORT->Group[port].OUTTGL.reg = (1<<pin);
	CRITICAL_SECTION_LEAVE()
}

void samgpio_setPinPullMode(const uint32_t gpio, const enum gpio_pull_mode pull_mode) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	switch(pull_mode) {
		case GPIO_PULL_OFF:
			PORT->Group[port].PINCFG[pin].bit.PULLEN = 0;
			break;
		case GPIO_PULL_UP:
			PORT->Group[port].PINCFG[pin].bit.PULLEN = 1;
			PORT->Group[port].OUTSET.reg = (1<<pin);
			break;
		case GPIO_PULL_DOWN:
			PORT->Group[port].PINCFG[pin].bit.PULLEN = 1;
			PORT->Group[port].OUTCLR.reg = (1<<pin);
			break;
		default:
			DEBUG_printf( ("ERROR: GPIO Pull Mode not supported.\n") );
	}
	CRITICAL_SECTION_LEAVE()
}