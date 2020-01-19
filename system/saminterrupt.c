/*
 * saminterrupt.c
 *
 * Created: 2019-09-23 4:16:17 PM
 *  Author: charl
 */ 

#include "saminterrupt.h"

IRQn_Type sam_getModuleInterrupt(void* const module) {
	uint32_t addr = (uint32_t)module;
	#ifdef SAME5x
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
			case (uint32_t)SERCOM6:
				return SERCOM6_IRQn;
			case (uint32_t)SERCOM7:
				return SERCOM7_IRQn;
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