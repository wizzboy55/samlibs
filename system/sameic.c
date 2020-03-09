/*
 * sameic.c
 *
 * Created: 2020-03-09 2:05:26 PM
 *  Author: charl
 */ 

#include "sameic.h"

#include "FreeRTOS.h"
#include "samclk.h"

#if defined (SAMC20)
#define EIC_SINGLECHANNELINTERRUPT
#endif

#if defined (EIC_SINGLECHANNELINTERRUPT)
EICCallback_t eicCallbacks[EIC_OFF];
#endif

void sam_EICInit(uint8_t clksource) {
	samclk_enable_gclk_channel(EIC, clksource);
	samclk_enable_peripheral_clock(EIC);
	EIC->CTRLA.bit.ENABLE = 1;
}

uint32_t sam_EICGetIRQn(eEICChannels_t channel) {
	#if defined (EIC_SINGLECHANNELINTERRUPT)
		return EIC_IRQn;
	#else
		return channel + EIC_0_IRQn;
	#endif
}

inline void sam_EICEnableExtIRQ(eEICChannels_t channel) {
	EIC->INTENSET.bit.EXTINT = (1<<channel);
	NVIC_EnableIRQ(sam_EICGetIRQn(channel));
}

inline void sam_EICDisableExtIRQ(eEICChannels_t channel) {
	EIC->INTENCLR.bit.EXTINT = (1<<channel);
	NVIC_DisableIRQ(sam_EICGetIRQn(channel));
}

inline void sam_EICClearIRQ(eEICChannels_t channel) {
	EIC->INTFLAG.reg |= (1<<channel);
}

void sam_EICConfigExt(eEICChannels_t channel, BaseType_t debounce, BaseType_t filter, eEICSense_t sense) {
	
	BaseType_t wasEnabled = EIC->CTRLA.bit.ENABLE == 1;
	
	EIC->CTRLA.bit.ENABLE = 0;
	
	#if defined (EIC_SINGLECHANNELINTERRUPT)
		// TODO Support Debounce on N variants
		if(debounce) {
			EIC->CTRLA.bit.CKSEL = 1;
		}
	#else
		EIC->DEBOUNCEN.bit.DEBOUNCEN |= ((debounce == pdTRUE) << channel);
	#endif

	if(channel < EIC8) {
		EIC->CONFIG[0].reg &= (0x0F << (channel * 4) ^ 0xFFFFFFFF);
		EIC->CONFIG[0].reg |= ((((filter == pdTRUE) << 3) | (sense & 0x07)) & 0x0F) << (channel * 4);
	} else {
		EIC->CONFIG[1].reg &= (0x0F << (channel * 4) ^ 0xFFFFFFFF);
		EIC->CONFIG[1].reg |= ((((filter == pdTRUE) << 3) | (sense & 0x07)) & 0x0F) << ((channel - EIC8) * 4);
	}
	
	if(wasEnabled) {
		EIC->CTRLA.bit.ENABLE = 1;
	}
}

eEICChannels_t sam_getGPIOEICChannel(GPIOPin_t gpio) {
	if(gpio == GPIO(GPIO_PORTA, 8)) {
		return EIC_NMI;
	}
	
	uint8_t pin = GPIO_PIN(gpio);
	
	return (eEICChannels_t)(pin & 0xF);
}

#if defined (EIC_SINGLECHANNELINTERRUPT)

void sam_EICSetCallback(eEICChannels_t channel, EICCallback_t cb) {
	sam_EICInit(0);
	eicCallbacks[channel] = cb;
}

void EIC_Handler(void) {
	for(uint8_t channel = 0; channel < EIC_NMI; channel++) {
		if(EIC->INTFLAG.bit.EXTINT & 1 << channel) {
			if(eicCallbacks[channel] != NULL) {
				eicCallbacks[channel](channel);
			}
			sam_EICClearIRQ(channel);
		}
	}
}

#endif

