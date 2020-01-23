/*
 * debug_interface.c
 *
 * Created: 2019-04-12 3:33:36 PM
 *  Author: Wizz
 */ 

#include "debug_interface.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#include "samclk.h"
#include "samgpio.h"
#include "HardwareDescriptor.h"

#define DEBUG_BUFSIZE 128
char buf[DEBUG_BUFSIZE];

usartConfig_t* debug_usart_config;

void debug_putchar(char c) {
	
	Sercom* sercomdevice = (Sercom *)debug_usart_config->sercom;
	
	if(c == '\n') {
		debug_putchar('\r');
	}
	while(sercomdevice->USART.INTFLAG.bit.DRE == 0) {
		
	}
	sercomdevice->USART.DATA.reg = c;
}

void debug_verbose(uint8_t verbose, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	#if(DEBUG_UART == 1)
		if(verbose <= DEBUG_VERBOSE_LEVEL) {
			vsnprintf(buf, DEBUG_BUFSIZE, fmt, args);
			const char *str = buf;
			while(*str != 0x00) {
				debug_putchar(*str++);
			}
		}
	#endif
	va_end(args);
}

void debug_printf(const char *fmt, ...) {
#if DEBUG_VERBOSE_LEVEL > 0
	#if DEBUG_UART == 1
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, DEBUG_BUFSIZE, fmt, args);
		const char *str = buf;
		while(*str != 0x00) {
			debug_putchar(*str++);
		}
		va_end(args);
	#endif
#endif
}

void debug_putstring(const char *str) {
	while(*str != 0x00) {
		debug_putchar(*str++);
	}
}

void debug_init(usartConfig_t* config) {

	#if DEBUG_UART == 1
		debug_usart_config = config;	

		Sercom* sercomdevice = (Sercom *)config->sercom;
	
		if(sercomdevice->USART.CTRLA.bit.MODE == SERCOM_USART_CTRLA_MODE(0x01)) {
			return;
		}

		samclk_enable_peripheral_clock(sercomdevice);
		samclk_enable_gclk_channel(sercomdevice, config->clksource);

		sercomdevice->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;

		// Wait while reset is in effect
		while(sercomdevice->USART.CTRLA.bit.SWRST) {

		}

		sercomdevice->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN;

		sercomdevice->USART.BAUD.reg = 65536 - ((65536 * 16.0f * config->baudrate) / CONF_CPU_FREQUENCY);

		sercomdevice->USART.INTENCLR.reg = 0xFF;

		sercomdevice->USART.INTENSET.bit.DRE = 1;

		sercomdevice->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(0x01) | SERCOM_USART_CTRLA_RXPO(0x01) | SERCOM_USART_CTRLA_ENABLE | SERCOM_USART_CTRLA_DORD;

		samgpio_setPinFunction(config->pin_tx, config->pinmux);
	#endif
}