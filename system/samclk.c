/*
 * samclk.c
 *
 * Created: 2019-08-08 9:26:37 AM
 *  Author: Wizz
 */ 

#include "samclk.h"

#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#if defined(SAME54)
	#include <hri_gclk_e54.h>
#endif

#include "debug_interface.h"

typedef enum {BUS_UNKNOWN, BUS_AHB, BUS_APBA, BUS_APBB, BUS_APBC, BUS_APBD} ClockBus_t;

ClockBus_t samclk_get_peripheral_bus(const void *const module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#if defined(SAME54) || defined(SAME53)
		switch(intmodule) {
			case (uint32_t)SERCOM0: return BUS_APBA;
			case (uint32_t)SERCOM1: return BUS_APBA;
			case (uint32_t)SERCOM2: return BUS_APBB;
			case (uint32_t)SERCOM3: return BUS_APBB;
			case (uint32_t)SERCOM4: return BUS_APBD;
			case (uint32_t)SERCOM5: return BUS_APBD;
		#ifdef SERCOM6
			case (uint32_t)SERCOM6: return BUS_APBD;
		#endif
		#ifdef SERCOM7
			case (uint32_t)SERCOM7: return BUS_APBD;
		#endif
			case (uint32_t)GMAC: return BUS_APBC;
			case (uint32_t)TC0: return BUS_APBA;
			case (uint32_t)TC1: return BUS_APBA;
			case (uint32_t)TC2: return BUS_APBB;
			case (uint32_t)TC3: return BUS_APBB;
			case (uint32_t)TC4: return BUS_APBC;
			case (uint32_t)TC5: return BUS_APBC;
		#ifdef TC6
			case (uint32_t)TC6: return BUS_APBD;
		#endif
		#ifdef TC7
			case (uint32_t)TC7: return BUS_APBD;
		#endif
			case (uint32_t)TCC0: return BUS_APBB;
			case (uint32_t)TCC1: return BUS_APBB;
			case (uint32_t)TCC2: return BUS_APBC;
			case (uint32_t)TCC3: return BUS_APBC;
			case (uint32_t)TCC4: return BUS_APBD;
			case (uint32_t)EIC:	 return BUS_APBA;
			case (uint32_t)DAC:	 return BUS_APBD;
			default: return BUS_UNKNOWN;
		}
	#elif defined SAMD20
		switch(intmodule) {
			case (uint32_t)SERCOM0: return BUS_APBC;
			case (uint32_t)SERCOM1: return BUS_APBC;
			case (uint32_t)SERCOM2: return BUS_APBC;
			case (uint32_t)SERCOM3: return BUS_APBC;
			case (uint32_t)SERCOM4: return BUS_APBC;
			case (uint32_t)SERCOM5: return BUS_APBC;
			default: return BUS_UNKNOWN;
		}
	#elif defined SAMC20
		switch(intmodule) {
			case (uint32_t)SERCOM0: return BUS_APBC;
			case (uint32_t)SERCOM1: return BUS_APBC;
			case (uint32_t)SERCOM2: return BUS_APBC;
			case (uint32_t)SERCOM3: return BUS_APBC;
			case (uint32_t)EIC:		return BUS_APBA;
			default: return BUS_UNKNOWN;
		}
	#else
		#error "CPU Architecture not supported"
	#endif
}

uint32_t samclk_get_peripheral_mask(const void *const module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#if defined(SAME54) || defined(SAME53)
		switch(intmodule) {
			case (uint32_t)SERCOM0: return MCLK_APBAMASK_SERCOM0;
			case (uint32_t)SERCOM1: return MCLK_APBAMASK_SERCOM1;
			case (uint32_t)SERCOM2: return MCLK_APBBMASK_SERCOM2;
			case (uint32_t)SERCOM3: return MCLK_APBBMASK_SERCOM3;
			case (uint32_t)SERCOM4: return MCLK_APBDMASK_SERCOM4;
			case (uint32_t)SERCOM5: return MCLK_APBDMASK_SERCOM5;
		#ifdef SERCOM6
			case (uint32_t)SERCOM6: return MCLK_APBDMASK_SERCOM6;
		#endif
		#ifdef SERCOM7
			case (uint32_t)SERCOM7: return MCLK_APBDMASK_SERCOM7;
		#endif
			case (uint32_t)GMAC:	return MCLK_APBCMASK_GMAC;
			case (uint32_t)TC0:		return MCLK_APBAMASK_TC0;
			case (uint32_t)TC1:		return MCLK_APBAMASK_TC1;
			case (uint32_t)TC2:		return MCLK_APBBMASK_TC2;
			case (uint32_t)TC3:		return MCLK_APBBMASK_TC3;
			case (uint32_t)TC4:		return MCLK_APBCMASK_TC4;
			case (uint32_t)TC5:		return MCLK_APBCMASK_TC5;
		#ifdef TC6
			case (uint32_t)TC6:		return MCLK_APBDMASK_TC6;
		#endif
		#ifdef TC7
			case (uint32_t)TC7:		return MCLK_APBDMASK_TC7;
		#endif
			case (uint32_t)TCC0:	return MCLK_APBBMASK_TCC0;
			case (uint32_t)TCC1:	return MCLK_APBBMASK_TCC1;
			case (uint32_t)TCC2:	return MCLK_APBCMASK_TCC2;
			case (uint32_t)TCC3:	return MCLK_APBCMASK_TCC3;
			case (uint32_t)TCC4:	return MCLK_APBDMASK_TCC4;
			case (uint32_t)EIC:		return MCLK_APBAMASK_EIC;
			case (uint32_t)DAC:		return MCLK_APBDMASK_DAC;
			default: return 0;
		}
	#elif defined SAMD20
		switch(intmodule) {
			case (uint32_t)SERCOM0: return PM_APBCMASK_SERCOM0;
			case (uint32_t)SERCOM1: return PM_APBCMASK_SERCOM1;
			case (uint32_t)SERCOM2: return PM_APBCMASK_SERCOM2;
			case (uint32_t)SERCOM3: return PM_APBCMASK_SERCOM3;
			case (uint32_t)SERCOM4: return PM_APBCMASK_SERCOM4;
			case (uint32_t)SERCOM5: return PM_APBCMASK_SERCOM5;
			default: return 0;
		}
	#elif defined SAMC20
		switch(intmodule) {
			case (uint32_t)SERCOM0: return MCLK_APBCMASK_SERCOM0;
			case (uint32_t)SERCOM1: return MCLK_APBCMASK_SERCOM1;
			case (uint32_t)SERCOM2: return MCLK_APBCMASK_SERCOM2;
			case (uint32_t)SERCOM3: return MCLK_APBCMASK_SERCOM3;
			case (uint32_t)EIC:		return MCLK_APBAMASK_EIC;
			default: return 0;
		}
	#else
		#error "CPU Architecture not supported"
	#endif
}

void samclk_enable_peripheral_clock(const void *const module) {
	
	ClockBus_t bus = samclk_get_peripheral_bus(module);
	uint32_t peripheral_mask = samclk_get_peripheral_mask(module);

	if(bus == BUS_UNKNOWN || peripheral_mask == 0) {
		DEBUG_printf( ("MCLK ERROR: Unsupported peripheral.\n") );
	}

	#if defined(SAME54) || defined(SAME53)
		CRITICAL_SECTION_ENTER();
		switch(bus) {
			case BUS_AHB:
			MCLK->AHBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBA:
			MCLK->APBAMASK.reg |= peripheral_mask;
			break;
			case BUS_APBB:
			MCLK->APBBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBC:
			MCLK->APBCMASK.reg |= peripheral_mask;
			break;
			case BUS_APBD:
			MCLK->APBDMASK.reg |= peripheral_mask;
			break;
			default:
			DEBUG_printf( ("MCLK ERROR: Unsupported clock bus.\n") );
			break;
		}
		CRITICAL_SECTION_LEAVE();
	#elif defined SAMD20
		CRITICAL_SECTION_ENTER();
		switch(bus) {
			case BUS_AHB:
			PM->AHBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBA:
			PM->APBAMASK.reg |= peripheral_mask;
			break;
			case BUS_APBB:
			PM->APBBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBC:
			PM->APBCMASK.reg |= peripheral_mask;
			break;
			default:
			DEBUG_printf( ("PM ERROR: Unsupported clock bus.\n") );
			break;
		}
		CRITICAL_SECTION_LEAVE();
	#elif defined SAMC20
		CRITICAL_SECTION_ENTER();
		switch(bus) {
			case BUS_AHB:
			MCLK->AHBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBA:
			MCLK->APBAMASK.reg |= peripheral_mask;
			break;
			case BUS_APBB:
			MCLK->APBBMASK.reg |= peripheral_mask;
			break;
			case BUS_APBC:
			MCLK->APBCMASK.reg |= peripheral_mask;
			break;
			default:
			DEBUG_printf( ("MCLK ERROR: Unsupported clock bus.\n") );
			break;
		}
		CRITICAL_SECTION_LEAVE();
	#else
		#error "CPU Architecture not supported"
	#endif
}

uint8_t samclk_get_peripheral_clockid(const void *const module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#if defined(SAME54) || defined(SAME53)
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_CORE;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_CORE;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_CORE;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_CORE;
		case (uint32_t)SERCOM4: return SERCOM4_GCLK_ID_CORE;
		case (uint32_t)SERCOM5: return SERCOM5_GCLK_ID_CORE;
	#ifdef SERCOM6
		case (uint32_t)SERCOM6: return SERCOM6_GCLK_ID_CORE;
	#endif
	#ifdef SERCOM7
		case (uint32_t)SERCOM7: return SERCOM7_GCLK_ID_CORE;
	#endif
		case (uint32_t)GMAC:	return GMAC_CLK_AHB_ID;
		case (uint32_t)TC0:		return TC0_GCLK_ID;
		case (uint32_t)TC1:		return TC1_GCLK_ID;
		case (uint32_t)TC2:		return TC2_GCLK_ID;
		case (uint32_t)TC3:		return TC3_GCLK_ID;
		case (uint32_t)TC4:		return TC4_GCLK_ID;
		case (uint32_t)TC5:		return TC5_GCLK_ID;
	#ifdef TC6
		case (uint32_t)TC6:		return TC6_GCLK_ID;
	#endif
	#ifdef TC7
		case (uint32_t)TC7:		return TC7_GCLK_ID;
	#endif
		case (uint32_t)TCC0:	return TCC0_GCLK_ID;
		case (uint32_t)TCC1:	return TCC1_GCLK_ID;
		case (uint32_t)TCC2:	return TCC2_GCLK_ID;
		case (uint32_t)TCC3:	return TCC3_GCLK_ID;
		case (uint32_t)TCC4:	return TCC4_GCLK_ID;
		case (uint32_t)EIC:		return EIC_GCLK_ID;
		case (uint32_t)DAC:		return DAC_GCLK_ID;
		default: return 0;
	}
	#elif defined SAMD20
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_CORE;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_CORE;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_CORE;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_CORE;
		case (uint32_t)SERCOM4: return SERCOM4_GCLK_ID_CORE;
		case (uint32_t)SERCOM5: return SERCOM5_GCLK_ID_CORE;
		default: return 0;
	}
	#elif defined SAMC20
		switch(intmodule) {
			case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_CORE;
			case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_CORE;
			case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_CORE;
			case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_CORE;
			case (uint32_t)EIC:		return EIC_GCLK_ID;
			default: return 0;
		}
	#else
	#error "CPU Architecture not supported"
	#endif
}

uint8_t samclk_get_peripheral_slowclockid(const void *const module) {
	
	uint32_t intmodule = (uint32_t)module;
	
	#ifdef SAME54
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_SLOW;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_SLOW;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_SLOW;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_SLOW;
		case (uint32_t)SERCOM4: return SERCOM4_GCLK_ID_SLOW;
		case (uint32_t)SERCOM5: return SERCOM5_GCLK_ID_SLOW;
		case (uint32_t)SERCOM6: return SERCOM6_GCLK_ID_SLOW;
		case (uint32_t)SERCOM7: return SERCOM7_GCLK_ID_SLOW;
		case (uint32_t)GMAC:	return GMAC_CLK_AHB_ID;
		default: return 0;
	}
	#elif defined SAME53
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_SLOW;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_SLOW;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_SLOW;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_SLOW;
		case (uint32_t)SERCOM4: return SERCOM4_GCLK_ID_SLOW;
		case (uint32_t)SERCOM5: return SERCOM5_GCLK_ID_SLOW;
		case (uint32_t)GMAC:	return GMAC_CLK_AHB_ID;
		default: return 0;
	}
	#elif defined SAMD20
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_SLOW;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_SLOW;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_SLOW;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_SLOW;
		case (uint32_t)SERCOM4: return SERCOM4_GCLK_ID_SLOW;
		case (uint32_t)SERCOM5: return SERCOM5_GCLK_ID_SLOW;
		default: return 0;
	}
	#elif defined SAMC20
	switch(intmodule) {
		case (uint32_t)SERCOM0: return SERCOM0_GCLK_ID_SLOW;
		case (uint32_t)SERCOM1: return SERCOM1_GCLK_ID_SLOW;
		case (uint32_t)SERCOM2: return SERCOM2_GCLK_ID_SLOW;
		case (uint32_t)SERCOM3: return SERCOM3_GCLK_ID_SLOW;
		default: return 0;
	}
	#else
	#error "CPU Architecture not supported"
	#endif
}

void samclk_enable_gclk_channel(const void *const module, const uint8_t source) {

	uint8_t channel = samclk_get_peripheral_clockid(module);

	CRITICAL_SECTION_ENTER();
	#if defined(SAME54) || defined(SAME53)
		GCLK->PCHCTRL[channel].reg = (source | GCLK_PCHCTRL_CHEN);
	#elif defined SAMD20
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(channel) | GCLK_CLKCTRL_GEN(source) | GCLK_CLKCTRL_CLKEN);
	#elif defined SAMC20
		if(source < 9) {
			GCLK->GENCTRL[source].bit.GENEN = 1;
			if(channel < 41) {
				GCLK->PCHCTRL[channel].bit.GEN = source;
				GCLK->PCHCTRL[channel].bit.CHEN = 1;
			}
		}
	#else
		#error "CPU Architecture not supported"
	#endif
	CRITICAL_SECTION_LEAVE();
}

void samclk_enable_glck_slow_channel(const void *const module, const uint8_t slow_source) {
	uint8_t slowchannel = samclk_get_peripheral_slowclockid(module);
	
	CRITICAL_SECTION_ENTER();
	#if defined(SAME54) || defined(SAME53)
		GCLK->PCHCTRL[slowchannel].reg = (slow_source | GCLK_PCHCTRL_CHEN);
	#elif defined SAMD20
		GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(slowchannel) | GCLK_CLKCTRL_GEN(slow_source) | GCLK_CLKCTRL_CLKEN);
	#elif defined SAMC20
		if(slow_source < 9) {
			GCLK->GENCTRL[slow_source].bit.GENEN = 1;
			if(slowchannel < 41) {
				GCLK->PCHCTRL[slowchannel].bit.GEN = slow_source;
				GCLK->PCHCTRL[slowchannel].bit.CHEN = 1;
			}
		}
	#else
		#error "CPU Architecture not supported"
	#endif
	CRITICAL_SECTION_LEAVE();
}