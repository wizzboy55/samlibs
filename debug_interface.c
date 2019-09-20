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
#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#include <hal_gpio.h>
#include <hal_io.h>

#include <peripheral_clk_config.h>

#include "samclk.h"

#define DEBUG_SERCOM SERCOM2
#define DEBUG_SERCOM_CLK SERCOM2_GCLK_ID_CORE

#define DEBUG_SERCOM_BAUDRATE 115200

#define DEBUG_UART 1

#define DEBUG_BUFSIZE 128
char buf[DEBUG_BUFSIZE];

void debug_putchar(char c) {
	if(c == '\n') {
		debug_putchar('\r');
	}
	while(DEBUG_SERCOM->USART.INTFLAG.bit.DRE == 0) {
		
	}
	DEBUG_SERCOM->USART.DATA.reg = c;
}

void debug_verbose(uint8_t verbose, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	#if(DEBUG_UART == 1)
		if(verbose <= DEBUG_VERBOSE_LEVEL) {
			debug_init();
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
		debug_init();
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

void debug_init(void) {

	if(DEBUG_SERCOM->USART.CTRLA.bit.MODE == SERCOM_USART_CTRLA_MODE(0x01)) {
		return;
	}

	samclk_enable_peripheral_clock(DEBUG_SERCOM);
	samclk_enable_gclk_channel(DEBUG_SERCOM, 0);

	DEBUG_SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;

	// Wait while reset is in effect
	while(DEBUG_SERCOM->USART.CTRLA.bit.SWRST) {

	}

	DEBUG_SERCOM->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN;

	DEBUG_SERCOM->USART.BAUD.reg = 65536 - ((65536 * 16.0f * DEBUG_SERCOM_BAUDRATE) / CONF_CPU_FREQUENCY);

	DEBUG_SERCOM->USART.INTENCLR.reg = 0xFF;

	DEBUG_SERCOM->USART.INTENSET.bit.DRE = 1;

	DEBUG_SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(0x01) | SERCOM_USART_CTRLA_RXPO(0x01) | SERCOM_USART_CTRLA_ENABLE | SERCOM_USART_CTRLA_DORD;

	gpio_set_pin_function(GPIO(GPIO_PORTB, 25), PINMUX_PB25D_SERCOM2_PAD0);
	
}