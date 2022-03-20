/*
 * saminterrupt.h
 *
 * Created: 2019-09-23 4:16:10 PM
 *  Author: charl
 */ 


#ifndef SAMINTERRUPT_H_
#define SAMINTERRUPT_H_

#include "FreeRTOS.h"
#include "samgpio.h"

#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#if defined(SAME54)
	#include <hri_gclk_e54.h>
#endif

typedef enum {PRIORITY_HIGHEST = 0, PRIORITY_HIGH = 1, PRIORITY_LOW = 2, PRIORITY_LOWEST = 3} InterruptPriority_t;

typedef void (*InterruptHandler_t)(void);

void sam_enableModuleInterrupt(void* const module);
void sam_disableModuleInterrupt(void* const module);
void sam_clearPendingModuleInterrupt(void* const module);
void sam_setModuleInterruptPriority(void* const module, InterruptPriority_t priority);
void sam_setModuleSubInterruptPriority(void* const module, uint8_t offset, InterruptPriority_t priority);
void sam_enableModuleSubInterrupt(void* const module, uint8_t offset);
void sam_disableModuleSubInterrupt(void* const module, uint8_t offset);

#endif /* SAMINTERRUPT_H_ */