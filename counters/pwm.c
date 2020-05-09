/*
 * pwm.c
 *
 * Created: 2020-01-22 9:04:28 AM
 *  Author: charl
 */ 

#include "pwm.h"

#include "samgpio.h"
#include "samclk.h"

BaseType_t pwm_init(pwmConfig_t* config) {
	Tc* tcdevice = (Tc *)config->module;
	
	if(tcdevice->COUNT8.CTRLA.bit.ENABLE == 1) {
		return pdFAIL;
	}
	
	samclk_enable_gclk_channel(config->module, config->clksource);
	samclk_enable_peripheral_clock(config->module);
	
	if(config->gpio_wo0 != GPIO_NONE) {
		samgpio_setPinFunction(config->gpio_wo0, samgpio_getModulePinmux(config->module));
	}
	if(config->gpio_wo1 != GPIO_NONE) {
		samgpio_setPinFunction(config->gpio_wo1, samgpio_getModulePinmux(config->module));
	}
	
	tcdevice->COUNT32.CTRLA.bit.MODE = config->counterWidth;
	
	tcdevice->COUNT32.WAVE.bit.WAVEGEN = TC_WAVE_WAVEGEN_NPWM_Val;
	
	tcdevice->COUNT32.CTRLA.bit.ENABLE = 1;
	
	return pdPASS;
}

void pwm_setEnable(pwmConfig_t* config, bool enable)
{
	Tc* tcdevice = (Tc *)config->module;
	tcdevice->COUNT32.CTRLA.bit.ENABLE = enable;
}

uint32_t* pwm_getPwmCompareRegister(pwmConfig_t* config, uint8_t channel) {
	Tc* tcdevice = (Tc *)config->module;
	
	if(channel == 0 || channel == 1) {
		switch(config->counterWidth) {
			case PWM_COUNT8:
				return (uint32_t*)&(tcdevice->COUNT8.CC[channel].reg);
			case PWM_COUNT16:
				return (uint32_t*)&(tcdevice->COUNT16.CC[channel].reg);
			case PWM_COUNT32:
				return &(tcdevice->COUNT32.CC[channel].reg);
		}
	}
	
	return NULL;
}

uint8_t* pwm_getPwmPeriodRegister(pwmConfig_t* config) {
	Tc* tcdevice = (Tc *)config->module;
	return &(tcdevice->COUNT8.PER.reg);
}

// Set PWM duty cycle for specified channel
//	- duty_16bit : 0xFFFF == 100%, 0 = 0%
void pwm_setDutyCycle(pwmConfig_t* config, uint8_t channel, uint16_t duty_16bit) {
	Tc* tcdevice = (Tc *)config->module;
	
	if(channel == 0 || channel == 1) {
		switch(config->counterWidth) {
			case PWM_COUNT8:
			{
				uint8_t period = tcdevice->COUNT8.PER.reg;
				uint8_t compare = (uint32_t)duty_16bit * period / 0xFFFF;
				tcdevice->COUNT8.CC[channel].reg = compare;
			}
				break;
			case PWM_COUNT16:
				tcdevice->COUNT16.CC[channel].reg = duty_16bit;
				break;
			case PWM_COUNT32:
				if(duty_16bit == 0)
					tcdevice->COUNT32.CC[channel].reg = 0; // 0%
				else if(duty_16bit == 0xFFFF)
					tcdevice->COUNT32.CC[channel].reg = 0xFFFFFFFF; // 100%
				else
					tcdevice->COUNT32.CC[channel].reg = ((uint32_t)duty_16bit << 16) | 0x7FFF; // Fit it on 32 bits
				break;
		}
	}
}
