/*
 * pwm.c
 *
 * Created: 2020-01-22 9:04:28 AM
 *  Author: charl
 */ 

#include "pwm.h"

#include "samgpio.h"
#include "samclk.h"

BaseType_t xPwmInitTc(PwmConfig_t* config) {
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

BaseType_t xPwmInitTcc(PwmConfig_t* config) {
	Tcc* tccdevice = (Tcc *)config->module;
	
	if(tccdevice->CTRLA.bit.ENABLE == 1) {
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
	if(config->gpio_wo2 != GPIO_NONE) {
		samgpio_setPinFunction(config->gpio_wo2, samgpio_getModulePinmux(config->module));
	}
	if(config->gpio_wo3 != GPIO_NONE) {
		samgpio_setPinFunction(config->gpio_wo3, samgpio_getModulePinmux(config->module));
	}
	
	tccdevice->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_NPWM_Val;
	
	tccdevice->CTRLA.bit.ENABLE = 1;
	
	return pdPASS;
}

uint32_t* pulPwmGetPwmCompareRegisterTc(PwmConfig_t* config, uint8_t channel) {
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

uint32_t* pulPwmGetPwmCompareRegisterTcc(PwmConfig_t* config, uint8_t channel) {
	Tcc* tcdevice = (Tcc *)config->module;
	
	if(channel >= 0 && channel <= 5) {
			return (uint32_t*)&(tcdevice->CC[channel].reg);
	}
	
	return NULL;
}

uint8_t* pucPwmGetPwmPeriodRegister(PwmConfig_t* config) {
	Tc* tcdevice = (Tc *)config->module;
	return &(tcdevice->COUNT8.PER.reg);
}