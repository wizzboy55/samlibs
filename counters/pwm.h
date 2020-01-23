/*
 * pwm.h
 *
 * Created: 2020-01-22 9:04:21 AM
 *  Author: charl
 */ 


#ifndef PWM_H_
#define PWM_H_

#include "FreeRTOS.h"
#include <stdint.h>

typedef enum {PWM_COUNT16 = 0, PWM_COUNT8, PWM_COUNT32} PWMCounterWidth_t;

typedef struct {
	void* module;
	uint8_t gpio_wo0;
	uint8_t gpio_wo1;
	uint8_t pinmux_wo0;
	uint8_t pinmux_wo1;
	uint8_t clksource;
	PWMCounterWidth_t counterWidth;
} pwmConfig_t;

BaseType_t pwm_init(pwmConfig_t* config);
uint32_t* pwm_getPwmCompareRegister(pwmConfig_t* config, uint8_t channel);

#endif /* PWM_H_ */