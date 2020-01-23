/*
 * saminterrupt.h
 *
 * Created: 2019-09-23 4:16:10 PM
 *  Author: charl
 */ 


#ifndef SAMINTERRUPT_H_
#define SAMINTERRUPT_H_

#include "FreeRTOS.h"

#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#if defined(SAME54)
	#include <hri_gclk_e54.h>
#endif

typedef enum {EIC0 = 0, EIC1, EIC2, EIC3, EIC4, EIC5, EIC6, EIC7, EIC8, EIC9, EIC10, EIC11, EIC12, EIC13, EIC14, EIC15} eEICChannels_t;
typedef enum {EIC_None = 0, EIC_Rise, EIC_Fall, EIC_Both, EIC_High, EIC_Low} eEICSense_t;

void sam_enableModuleInterrupt(void* const module);
void sam_disableModuleInterrupt(void* const module);
void sam_EICInit(uint8_t clksource);
void sam_EICEnableExtIRQ(eEICChannels_t channel);
void sam_EICDisableExtIRQ(eEICChannels_t channel);
void sam_EICConfigExt(eEICChannels_t channel, BaseType_t debouce, BaseType_t filter, eEICSense_t sense);

#endif /* SAMINTERRUPT_H_ */