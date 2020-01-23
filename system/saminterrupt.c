/*
 * saminterrupt.c
 *
 * Created: 2019-09-23 4:16:17 PM
 *  Author: charl
 */ 

#include "saminterrupt.h"
#include "samclk.h"

IRQn_Type sam_getModuleInterrupt(void* const module) {
	uint32_t addr = (uint32_t)module;
	#if defined(SAME54) || defined(SAME53)
		switch(addr) {
			case (uint32_t)SERCOM0:
				return SERCOM0_0_IRQn;
			case (uint32_t)SERCOM1:
				return SERCOM1_0_IRQn;
			case (uint32_t)SERCOM2:
				return SERCOM2_0_IRQn;
			case (uint32_t)SERCOM3:
				return SERCOM3_0_IRQn;
			case (uint32_t)SERCOM4:
				return SERCOM4_0_IRQn;
			case (uint32_t)SERCOM5:
				return SERCOM5_0_IRQn;
			#ifdef SERCOM6
			case (uint32_t)SERCOM6:
				return SERCOM6_0_IRQn;
			#endif
			#ifdef SERCOM7
			case (uint32_t)SERCOM7:
				return SERCOM7_0_IRQn;
			#endif
		}
	#elif defined SAMD20
		switch(addr) {
			case (uint32_t)SERCOM0:
				return SERCOM0_IRQn;
			case (uint32_t)SERCOM1:
				return SERCOM1_IRQn;
			case (uint32_t)SERCOM2:
				return SERCOM2_IRQn;
			case (uint32_t)SERCOM3:
				return SERCOM3_IRQn;
			case (uint32_t)SERCOM4:
				return SERCOM4_IRQn;
			case (uint32_t)SERCOM5:
				return SERCOM5_IRQn;
		}
	#elif defined SAMC20
		switch(addr) {
			case (uint32_t)SERCOM0:
				return SERCOM0_IRQn;
			case (uint32_t)SERCOM1:
				return SERCOM1_IRQn;
			case (uint32_t)SERCOM2:
				return SERCOM2_IRQn;
			case (uint32_t)SERCOM3:
				return SERCOM3_IRQn;
		}
	#else
		#error "Device not supported."
	#endif
	
	return NonMaskableInt_IRQn;
}

void sam_enableModuleInterrupt(void* const module) {
	NVIC_EnableIRQ(sam_getModuleInterrupt(module));
}

void sam_disableModuleInterrupt(void* const module) {
	NVIC_DisableIRQ(sam_getModuleInterrupt(module));
}

void sam_EICInit(uint8_t clksource) {
	samclk_enable_gclk_channel(EIC, clksource);
	samclk_enable_peripheral_clock(EIC);
	EIC->CTRLA.bit.ENABLE = 1;
}

inline void sam_EICEnableExtIRQ(eEICChannels_t channel) {
	EIC->INTENSET.bit.EXTINT = (1<<channel);
}

inline void sam_EICDisableExtIRQ(eEICChannels_t channel) {
	EIC->INTENCLR.bit.EXTINT = (1<<channel);
}

void sam_EICConfigExt(eEICChannels_t channel, BaseType_t debouce, BaseType_t filter, eEICSense_t sense) {
	EIC->DEBOUNCEN.bit.DEBOUNCEN |= ((debouce == pdTRUE) << channel);
	if(channel < EIC8) {
		EIC->CONFIG[0].reg = ((((filter == pdTRUE) << 3) | (sense & 0x07)) & 0x0F) << (channel * 4);
	} else {
		EIC->CONFIG[1].reg = ((((filter == pdTRUE) << 3) | (sense & 0x07)) & 0x0F) << ((channel - EIC8) * 4);
	}
}