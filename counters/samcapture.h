/*
 * samcapture.h
 *
 * Created: 2020-06-30 10:30:09 AM
 *  Author: charl
 */ 


#ifndef SAMCAPTURE_H_
#define SAMCAPTURE_H_

#include "FreeRTOS.h"
#include <stdint.h>
#include "samgpio.h"

#include "samevent.h"
#include "pwm.h"

typedef struct {
	void* module;
	GPIOPin_t gpio;
	uint8_t pull;
	uint8_t clksource;
	BaseType_t invert;
	PWMCounterWidth_t counterWidth;
	EventConfig_t eventConfig;
} CaptureConfig_t;

BaseType_t xSamCaptureInitPPW(CaptureConfig_t* config);
void vSamCaptureGetPPW(CaptureConfig_t* config, uint32_t* period, uint32_t* duty);


#endif /* SAMCAPTURE_H_ */