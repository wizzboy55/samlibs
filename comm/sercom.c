/*
 * sercom.c
 *
 * Created: 2019-09-23 3:07:48 PM
 *  Author: charl
 */ 

#include "sercom.h"
#include "FreeRTOS.h"

enum SercomPads_e eSercomGPIOToPad(GPIOPin_t gpio) {
#if defined(SAMC20)
	switch(gpio) {
		case GPIO(GPIO_PORTA, 0):
		case GPIO(GPIO_PORTA, 4):
		case GPIO(GPIO_PORTA, 8):
		case GPIO(GPIO_PORTA, 16):
		case GPIO(GPIO_PORTA, 22):
			return PAD0;
		case GPIO(GPIO_PORTA, 1):
		case GPIO(GPIO_PORTA, 5):
		case GPIO(GPIO_PORTA, 9):
		case GPIO(GPIO_PORTA, 17):
		case GPIO(GPIO_PORTA, 23):
			return PAD1;
		case GPIO(GPIO_PORTA, 6):
		case GPIO(GPIO_PORTA, 10):
		case GPIO(GPIO_PORTA, 14):
		case GPIO(GPIO_PORTA, 18):
		case GPIO(GPIO_PORTA, 24):
		case GPIO(GPIO_PORTA, 30):
			return PAD2;
		case GPIO(GPIO_PORTA, 7):
		case GPIO(GPIO_PORTA, 11):
		case GPIO(GPIO_PORTA, 15):
		case GPIO(GPIO_PORTA, 19):
		case GPIO(GPIO_PORTA, 25):
		case GPIO(GPIO_PORTA, 31):
			return PAD3;
		default:
			return -1;
	}
#endif
}

int8_t xSercomPadToRXPO(enum SercomPads_e pad) {
	switch(pad) {
		case PAD0:
			return 0;
		case PAD1:
			return 1;
		case PAD2:
			return 2;
		case PAD3:
			return 3;
		default:
			return -1;
	}
}

int8_t xSercomPadToTXPO(enum SercomPads_e pad) {
	switch(pad) {
		case PAD0:
			return 0;
		case PAD2:
			return 1;
		default:
			return -1;
	}
}