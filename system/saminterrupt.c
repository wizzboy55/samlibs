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
	#if defined(SAME54) || defined(SAME53) || defined(SAMD51)
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
			case (uint32_t)TC0:
				return TC0_IRQn;
			case (uint32_t)TC1:
				return TC1_IRQn;
			case (uint32_t)TC2:
				return TC2_IRQn;
			case (uint32_t)TC3:
				return TC3_IRQn;
			case (uint32_t)TC4:
				return TC4_IRQn;
		}
	#else
		#error "Device not supported."
	#endif
	
	return NonMaskableInt_IRQn;
}

void sam_enableModuleInterrupt(void* const module) {
	NVIC_EnableIRQ(sam_getModuleInterrupt(module));
}

void sam_enableModuleSubInterrupt(void* const module, uint8_t offset) {
	NVIC_EnableIRQ(sam_getModuleInterrupt(module) + offset);
}

void sam_disableModuleInterrupt(void* const module) {
	NVIC_DisableIRQ(sam_getModuleInterrupt(module));
}

void sam_disableModuleSubInterrupt(void* const module, uint8_t offset) {
	NVIC_DisableIRQ(sam_getModuleInterrupt(module) + offset);
}

void sam_clearPendingModuleInterrupt(void* const module) {
	NVIC_ClearPendingIRQ(sam_getModuleInterrupt(module));
}

void sam_setModuleInterruptPriority(void* const module, InterruptPriority_t priority) {
	NVIC_SetPriority(sam_getModuleInterrupt(module), (uint32_t)priority);
}

void sam_setModuleSubInterruptPriority(void* const module, uint8_t offset, InterruptPriority_t priority) {
	NVIC_SetPriority(sam_getModuleInterrupt(module) + offset, (uint32_t)priority);
}