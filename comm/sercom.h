/*
 * sercom.h
 *
 * Created: 2019-09-23 3:07:42 PM
 *  Author: charl
 */ 


#ifndef SERCOM_H_
#define SERCOM_H_

#include <stdint.h>

enum SercomPads_e {PAD0, PAD1, PAD2, PAD3};

enum SercomPads_e eSercomGPIOToPad(uint32_t gpio);
int8_t xSercomPadToRXPO(enum SercomPads_e pad);
int8_t xSercomPadToTXPO(enum SercomPads_e pad);

#endif /* SERCOM_H_ */