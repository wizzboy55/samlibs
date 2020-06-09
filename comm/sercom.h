/*
 * sercom.h
 *
 * Created: 2019-09-23 3:07:42 PM
 *  Author: charl
 */ 


#ifndef SERCOM_H_
#define SERCOM_H_

#include <stdint.h>

#include "samgpio.h"

enum SercomPads_e {PADNONE = -1, PAD0 = 0, PAD1, PAD2, PAD3};

enum SercomDataOrder_e {MSBFirst, LSBFirst};
	
enum SercomParity_e {EvenParity = 0, OddParity = 1, NoParity = 2};
	
enum SercomStopBits_e {OneStopBit = 0, TwoStopBit = 1};
	
enum SercomCharacterSize_e {Char8bit = 0, Char9bit = 1, Char5bit = 5, Char6bit = 6, Char7bit = 7};
enum SercomPads_e eSercomGPIOToPad(GPIOPin_t gpio);
int8_t xSercomPadToRXPO(enum SercomPads_e pad);
int8_t xSercomPadToTXPO(enum SercomPads_e pad);
int8_t xSercomPadToDIPO(enum SercomPads_e pad);
int8_t xSercomPadToDOPO(enum SercomPads_e pad_do, enum SercomPads_e pad_clk);
int8_t xSercomDOPOToDIPO(int8_t dopo);
uint8_t uSercomDataOrderToRegister(enum SercomDataOrder_e dataOrder);
uint8_t uSercomParityToFrameFormat(enum SercomParity_e parity);
uint8_t uSercomParityToRegister(enum SercomParity_e parity);

#endif /* SERCOM_H_ */