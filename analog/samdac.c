/*
 * samdac.c
 *
 * Created: 2020-01-23 10:00:28 AM
 *  Author: charl
 */ 

#include "samdac.h"

/*
void samdac_init(samDacConfig_t* config) {
	DAC->CTRLA.bit.ENABLE = 0;
	
	DAC->CTRLB.bit.DIFF = (config->differential == pdTRUE);
	DAC->CTRLB.bit.REFSEL = config->ref;
	DAC->DACCTRL[0].bit.OSR = config->dac0oversampling;
	DAC->DACCTRL[1].bit.OSR = config->dac1oversampling;
}
*/