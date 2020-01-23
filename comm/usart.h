/*
 * usart.h
 *
 * Created: 2020-01-21 4:39:20 PM
 *  Author: charl
 */ 


#ifndef USART_H_
#define USART_H_

typedef struct {
	void* sercom;
	uint32_t baudrate;
	uint8_t pin_tx;
	uint8_t pin_rx;
	uint8_t pinmux;
	uint8_t clksource;
} usartConfig_t;



#endif /* USART_H_ */