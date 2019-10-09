/*
 * saminterrupt.h
 *
 * Created: 2019-09-23 4:16:10 PM
 *  Author: charl
 */ 


#ifndef SAMINTERRUPT_H_
#define SAMINTERRUPT_H_

#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#if defined(SAME54)
	#include <hri_gclk_e54.h>
#endif

void sam_enableModuleInterrupt(void* const module);
void sam_disableModuleInterrupt(void* const module);

#endif /* SAMINTERRUPT_H_ */