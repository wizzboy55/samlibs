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

enum SercomPads_e eSercomGPIOToPad(GPIOPin_t gpio);
int8_t xSercomPadToRXPO(enum SercomPads_e pad);
int8_t xSercomPadToTXPO(enum SercomPads_e pad);
int8_t xSercomPadToDIPO(enum SercomPads_e pad);
int8_t xSercomPadToDOPO(enum SercomPads_e pad_do, enum SercomPads_e pad_clk);
int8_t xSercomDOPOToDIPO(int8_t dopo);

#endif /* SERCOM_H_ */