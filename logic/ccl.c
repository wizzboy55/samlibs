/*
 * ccl.c
 *
 * Created: 2020-03-09 9:46:50 AM
 *  Author: charl
 */ 

#include "ccl.h"
#include "samclk.h"
#include "samgpio.h"

void vCCLSetupPins(CCLConfig_t* config) {
#if defined(SAMC20)
	#define CCL_PINMUX 8
#endif
	
	if(config->in0pin != GPIO_NONE) {
		samgpio_setPinFunction(config->in0pin, CCL_PINMUX);
	}
	
	if(config->in1pin != GPIO_NONE) {
		samgpio_setPinFunction(config->in1pin, CCL_PINMUX);
	}
	
	if(config->in2pin != GPIO_NONE) {
		samgpio_setPinFunction(config->in2pin, CCL_PINMUX);
	}
	
	if(config->outpin != GPIO_NONE) {
		samgpio_setPinFunction(config->outpin, CCL_PINMUX);
	}
}

void vCCLInit(CCLConfig_t* config) {
	
	if(config->module > 3) {
		return;
	}
	
	CCL->CTRL.bit.ENABLE = 0;
	
	samclk_enable_gclk_channel(CCL, 0);
	samclk_enable_peripheral_clock(CCL);

	CCL->LUTCTRL[config->module].bit.TRUTH = config->truthtable;
	CCL->LUTCTRL[config->module].bit.INSEL0 = config->in0sel;
	CCL->LUTCTRL[config->module].bit.INSEL1 = config->in1sel;
	CCL->LUTCTRL[config->module].bit.INSEL2 = config->in2sel;
	CCL->LUTCTRL[config->module].bit.FILTSEL = config->filtersel;
	CCL->LUTCTRL[config->module].bit.EDGESEL = config->edgedetect == pdTRUE;
	CCL->LUTCTRL[config->module].bit.ENABLE = 1;
	
	vCCLSetupPins(config);
	
	CCL->CTRL.bit.ENABLE = 1;
	
}