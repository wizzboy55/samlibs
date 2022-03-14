/*
 * samgpio.c
 *
 * Created: 2019-08-17 4:25:38 PM
 *  Author: Wizz
 */ 

#include "samgpio.h"

#include "debug_interface.h"

void samgpio_setPinFunction(const GPIOPin_t gpio, const PinMux_t function) {
	if(gpio == GPIO_NONE) {
		return;
	}
	
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

void samgpio_setPinDirection(const GPIOPin_t gpio, const enum gpio_direction direction) {
	if(gpio == GPIO_NONE) {
		return;
	}
	
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	CRITICAL_SECTION_ENTER()
	switch (direction) {
		case GPIO_DIRECTION_OFF:
			PORT->Group[port].DIRCLR.reg = (1 << pin);
			PORT->Group[port].PINCFG[pin].bit.INEN = 0;
			PORT->Group[port].PINCFG[pin].bit.PULLEN = 0;
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

void samgpio_setPinLevel(const GPIOPin_t gpio, const uint8_t level) {
	if(gpio == GPIO_NONE) {
		return;
	}
	
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

inline void samgpio_setPinLevelFast(const GPIOPin_t gpio, const uint8_t level) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	if(level) {
		PORT->Group[port].OUTSET.reg = (1<<pin);
		} else {
		PORT->Group[port].OUTCLR.reg = (1<<pin);
	}
}

inline void samgpio_setPinFast(const GPIOPin_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	PORT->Group[port].OUTSET.reg = (1<<pin);
}

inline void samgpio_clearPinFast(const GPIOPin_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	PORT->Group[port].OUTCLR.reg = (1<<pin);
}

inline void samgpio_togglePinLevelFast(const GPIOPin_t gpio) {
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin  = GPIO_PIN(gpio);

	PORT->Group[port].OUTTGL.reg = (1<<pin);
}

void samgpio_setPinPullMode(const GPIOPin_t gpio, const enum gpio_pull_mode pull_mode) {
	if(gpio == GPIO_NONE) {
		return;
	}	

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

uint8_t samgpio_getPinLevel(const GPIOPin_t gpio) {
	if(gpio == GPIO_NONE) {
		return 0;
	}
	
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin = GPIO_PIN(gpio);
	uint32_t pinoffset  = (1<<pin);
	
	if(PORT->Group[port].PINCFG[pin].bit.INEN) {
		return (PORT->Group[port].IN.reg & pinoffset) > 0;
	} else {
		return (PORT->Group[port].OUT.reg & pinoffset) > 0;
	}
}

inline uint8_t samgpio_getPinLevelFast(const GPIOPin_t gpio) {
	
	uint8_t port = GPIO_PORT(gpio);
	uint8_t pin = GPIO_PIN(gpio);
	uint32_t pinoffset  = (1<<pin);
	
	if(PORT->Group[port].PINCFG[pin].bit.INEN) {
		return (PORT->Group[port].IN.reg & pinoffset) > 0;
	} else {
		return (PORT->Group[port].OUT.reg & pinoffset) > 0;
	}
}

PinMux_t samgpio_getModulePinmux(const void* module) {
#if defined(SAME54) || defined(SAME53) || defined(SAMD51)
	switch((uint32_t)module) {
		case (uint32_t)EIC:
			return 0;
		case (uint32_t)ADC0:
		case (uint32_t)ADC1:
		case (uint32_t)DAC:
		case (uint32_t)AC:
			return 1;
		case (uint32_t)TC0:
		case (uint32_t)TC1:
		case (uint32_t)TC2:
		case (uint32_t)TC3:
		case (uint32_t)TC4:
		case (uint32_t)TC5:
	#ifdef TC6
		case (uint32_t)TC6:
	#endif
	#ifdef TC7
		case (uint32_t)TC7:
	#endif
			return 4;
		case (uint32_t)TCC0:
		case (uint32_t)TCC1:
		case (uint32_t)TCC2:
		case (uint32_t)TCC3:
			return 5;
		case (uint32_t)CCL:
			return 13;
#if defined(SAME54) || defined(SAME53)
		case (uint32_t)GMAC:
			return 11;
#endif
		default:
			return GPIO_PIN_FUNCTION_OFF;
	}
#elif defined(SAMC20)
	switch((uint32_t)module) {
		case (uint32_t)EIC:
			return 0;
		case (uint32_t)ADC0:
	#ifdef ADC1
		case (uint32_t)ADC1:
	#endif
		case (uint32_t)AC:
	#ifdef DAC
		case (uint32_t)DAC:
	#endif
		case (uint32_t)PTC:
			return 1;
		case (uint32_t)TC0:
		case (uint32_t)TC1:
		case (uint32_t)TC2:
		case (uint32_t)TC3:
		case (uint32_t)TC4:
			return 4;
		case (uint32_t)CCL:
			return 8;
		default:
			return GPIO_PIN_FUNCTION_OFF;
	}
#endif
}