/*
 * samevent.h
 *
 * Created: 2020-06-30 11:55:43 AM
 *  Author: charl
 */ 


#ifndef SAMEVENT_H_
#define SAMEVENT_H_

#include <stdint.h>
#include "FreeRTOS.h"

typedef struct {
	void* user;
	uint8_t userOffset;
	void* generator;
	uint8_t generatorOffset;
	uint8_t channel;
} EventConfig_t;

BaseType_t xSamEventInit(EventConfig_t* config);

#endif /* SAMEVENT_H_ */