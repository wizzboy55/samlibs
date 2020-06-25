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
			return PADNONE;
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

int8_t xSercomPadToDIPO(enum SercomPads_e pad) {
	return xSercomPadToRXPO(pad);
};

int8_t xSercomDOPOToDIPO(int8_t dopo) {
	switch(dopo) {
		case 0:
			return 3;
		case 1:
			return 0;
		case 2:
			return 0;
		case 3:
			return 2;
		default:
			return -1;
	}
}

int8_t xSercomPadToDOPO(enum SercomPads_e pad_do, enum SercomPads_e pad_clk) {
	switch(pad_do) {
		case PAD0:
			switch(pad_clk) {
				case PAD1:
					return 0;
				case PAD3:
					return 3;
				default:
					return -1;
			}
		default:
		case PAD1:
			return -1;
		case PAD2:
			return 1;
		case PAD3:
			return 2;
		case PADNONE:
			switch(pad_clk) {
				case PAD1:
					return 0;
				case PAD3:
					return 1;
				default:
					return -1;
			}
	};
};

inline uint16_t ulSercomBaudrateToRegisterArithmetic(uint32_t baudrate) {
	return (65536 - ((65536 * 16.0f * baudrate) / CONF_CPU_FREQUENCY));
}

inline uint8_t uSercomBaudrateToRegisterSynchronous(uint32_t baudrate) {
	return (CONF_CPU_FREQUENCY / (2*baudrate)) - 1;
}

inline uint8_t uSercomDataOrderToRegister(enum SercomDataOrder_e dataOrder) {
	return dataOrder == LSBFirst;
}

inline uint8_t uSercomParityToFrameFormat(enum SercomParity_e parity) {
	switch(parity) {
		case NoParity:
		default:
			return 0x00;
		case EvenParity:
		case OddParity:
			return 0x01;
	}
}

inline uint8_t uSercomParityToRegister(enum SercomParity_e parity) {
	switch(parity) {
		case EvenParity:
		default:
			return 0x00;
		case OddParity:
			return 0x01;
	}
}