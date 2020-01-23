/*
 * debug_interface.h
 *
 * Created: 2019-04-12 3:33:25 PM
 *  Author: Wizz
 */ 


#ifndef DEBUG_INTERFACE_H_
#define DEBUG_INTERFACE_H_

#include <stdio.h>
#include "usart.h"

#if DEBUG_VERBOSE_LEVEL > 0
#define DEBUG_printf(X)	debug_printf X
#define DEBUG_verbose(X) debug_verbose X
#else
#define DEBUG_printf(X) do{} while(0)
#define DEBUG_verbose(X) do{} while(0)
#endif

void debug_init(usartConfig_t* config);

void debug_verbose(uint8_t verbose, const char *fmt, ...);

void debug_printf(const char *fmt, ...);

void debug_putstring(const char *str);

#endif /* DEBUG_INTERFACE_H_ */