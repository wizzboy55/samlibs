/*
 * samevent.c
 *
 * Created: 2020-06-30 11:55:51 AM
 *  Author: charl
 */ 

#include "samevent.h"

#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#if defined(SAME54)
#include <hri_gclk_e54.h>
#endif

#define EVSYS_CHANNEL_MAX 32
#define EVSYS_USERS_MAX 67

#include "samclk.h"

uint8_t ucSamEventGetGeneratorFromModule(void* module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#if defined(SAME54) || defined(SAME53)
		switch(intmodule) {
			case (uint32_t)EIC:		return 0x12;
			case (uint32_t)DMAC:	return 0x22;
			case (uint32_t)PAC:		return 0x26;
			case (uint32_t)TCC0:	return 0x29;
			case (uint32_t)TCC1:	return 0x32;
			case (uint32_t)TCC2:	return 0x39;
			case (uint32_t)TCC3:	return 0x3F;
			case (uint32_t)TCC4:	return 0x44;
			case (uint32_t)TC0:		return 0x49;
			case (uint32_t)TC1:		return 0x4C;
			case (uint32_t)TC2:		return 0x4F;
			case (uint32_t)TC3:		return 0x52;
			case (uint32_t)TC4:		return 0x55;
			case (uint32_t)TC5:		return 0x58;
		#ifdef TC6
			case (uint32_t)TC6:		return 0x5B;
		#endif
		#ifdef TC7
			case (uint32_t)TC7:		return 0x5E;
		#endif
			case (uint32_t)PDEC:	return 0x61;
			case (uint32_t)ADC0:	return 0x67;
			case (uint32_t)ADC1:	return 0x69;
			case (uint32_t)AC:		return 0x6B;
			case (uint32_t)DAC:		return 0x6E;
			case (uint32_t)GMAC:	return 0x72;
			case (uint32_t)TRNG:	return 0x73;
			case (uint32_t)CCL:		return 0x74;
			default:				return 0x00;
		}
	#endif
}

uint8_t ucSamEventGetUserFromModule(void* module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#if defined(SAME54) || defined(SAME53)
		switch(intmodule) {
			case (uint32_t)RTC:		return 00;
			case (uint32_t)PORT:	return 01;
			case (uint32_t)DMAC:	return 05;
			case (uint32_t)TCC0:	return 17;
			case (uint32_t)TCC1:	return 25;
			case (uint32_t)TCC2:	return 31;
			case (uint32_t)TCC3:	return 36;
			case (uint32_t)TCC4:	return 40;
			case (uint32_t)TC0:		return 44;
			case (uint32_t)TC1:		return 45;
			case (uint32_t)TC2:		return 46;
			case (uint32_t)TC3:		return 47;
			case (uint32_t)TC4:		return 48;
			case (uint32_t)TC5:		return 49;
		#ifdef TC6
			case (uint32_t)TC6:		return 50;
		#endif
		#ifdef TC7
			case (uint32_t)TC7:		return 51;
		#endif
			case (uint32_t)PDEC:	return 52;
			case (uint32_t)ADC0:	return 55;
			case (uint32_t)ADC1:	return 57;
			case (uint32_t)AC:		return 59;
			case (uint32_t)DAC:		return 61;
			case (uint32_t)CCL:		return 63;
			default:				return EVSYS_USERS_MAX;
		}
	#endif
}

BaseType_t xSamEventInit(EventConfig_t* config) {
	samclk_enable_peripheral_clock(EVSYS);
	
	uint8_t user = ucSamEventGetUserFromModule(config->user) + config->userOffset;
	
	if(config->channel < EVSYS_CHANNEL_MAX && user < EVSYS_USERS_MAX) {
		EVSYS->Channel[config->channel].CHANNEL.bit.EDGSEL = 0;
		EVSYS->Channel[config->channel].CHANNEL.bit.PATH = EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val;
		EVSYS->Channel[config->channel].CHANNEL.bit.EVGEN = ucSamEventGetGeneratorFromModule(config->generator) + config->generatorOffset;
		EVSYS->USER[user].bit.CHANNEL = config->channel + 1;
		return pdPASS;
	}
	
	return pdFAIL;
}