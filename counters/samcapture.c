/*
 * samcapture.c
 *
 * Created: 2020-06-30 10:29:54 AM
 *  Author: charl
 */ 

#include "samcapture.h"
#include "samclk.h"
#include "sameic.h"

BaseType_t xSamCaptureInitPPW(CaptureConfig_t* config) {
	Tc* tcdevice = (Tc *)config->module;
	
	if(tcdevice->COUNT8.CTRLA.bit.ENABLE == 1) {
		return pdFAIL;
	}
	
	samclk_enable_gclk_channel(config->module, config->clksource);
	samclk_enable_peripheral_clock(config->module);
	
	tcdevice->COUNT32.CTRLA.reg = 0x00;
	
	if(config->gpio == GPIO_NONE) {
		return pdFAIL;
	}
	
	samgpio_setPinFunction(config->gpio, 0);
	samgpio_setPinDirection(config->gpio, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(config->gpio, config->pull);
	
	// Setup Event
	sam_EICEnableEvent(sam_getGPIOEICChannel(config->gpio));
	sam_EICInit(config->clksource);
	sam_EICConfigExt(sam_getGPIOEICChannel(config->gpio), pdFALSE, pdFALSE, EIC_High);
	
	xSamEventInit(&(config->eventConfig));
	
	tcdevice->COUNT32.CTRLA.bit.CAPTEN0 = 1;
	tcdevice->COUNT32.CTRLA.bit.CAPTEN1 = 1;
	tcdevice->COUNT32.CTRLA.bit.COPEN0 = 0;
	tcdevice->COUNT32.CTRLA.bit.COPEN1 = 0;
	
	tcdevice->COUNT32.CTRLA.bit.MODE = config->counterWidth;
	tcdevice->COUNT32.CTRLA.bit.PRESCALER = TC_CTRLA_PRESCALER_DIV256_Val;
	
	tcdevice->COUNT32.EVCTRL.bit.EVACT = TC_EVCTRL_EVACT_PPW_Val;
	tcdevice->COUNT32.EVCTRL.bit.TCINV = (config->invert == pdTRUE);
	tcdevice->COUNT32.EVCTRL.bit.TCEI = 1;
	
	tcdevice->COUNT32.CTRLA.bit.ENABLE = 1;
	
	return pdPASS;
}

void vSamCaptureGetPPW(CaptureConfig_t* config, uint32_t* period, uint32_t* duty) {
	Tc* tcdevice = (Tc *)config->module;
	
	if(tcdevice->COUNT32.EVCTRL.bit.EVACT == TC_EVCTRL_EVACT_PPW_Val) {
		switch(config->counterWidth) {
			case PWM_COUNT8:
				*period = tcdevice->COUNT8.CC[0].reg;
				*duty = tcdevice->COUNT8.CC[1].reg;
				break;
			case PWM_COUNT16:
				*period = tcdevice->COUNT16.CC[0].reg;
				*duty = tcdevice->COUNT16.CC[1].reg;
				break;
			case PWM_COUNT32:
				*period = tcdevice->COUNT32.CC[0].reg;
				*duty = tcdevice->COUNT32.CC[1].reg;
				break;
		}
	} else if(tcdevice->COUNT32.EVCTRL.bit.EVACT == TC_EVCTRL_EVACT_PWP_Val) {
		switch(config->counterWidth) {
			case PWM_COUNT8:
				*period = tcdevice->COUNT8.CC[1].reg;
				*duty = tcdevice->COUNT8.CC[0].reg;
				break;
			case PWM_COUNT16:
				*period = tcdevice->COUNT16.CC[1].reg;
				*duty = tcdevice->COUNT16.CC[0].reg;
				break;
			case PWM_COUNT32:
				*period = tcdevice->COUNT32.CC[1].reg;
				*duty = tcdevice->COUNT32.CC[0].reg;
				break;
		}
	}
}

uint32_t* pulSamCaptureGetCCRegister(CaptureConfig_t* config, uint8_t channel) {

	
	return NULL;
}