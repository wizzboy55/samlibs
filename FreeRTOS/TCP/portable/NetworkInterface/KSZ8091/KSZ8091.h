/*
 * KSZ8091.h
 *
 * Created: 2019-07-29 2:04:30 PM
 *  Author: Wizz
 */ 


#ifndef KSZ8091_H_
#define KSZ8091_H_

#include "stdint.h"
#include "FreeRTOS.h"

BaseType_t xKSZ8091InitializePHY(uint8_t address);

#endif /* KSZ8091_H_ */