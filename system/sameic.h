/*
 * sameic.h
 *
 * Created: 2020-03-09 2:05:19 PM
 *  Author: charl
 */ 


#ifndef SAMEIC_H_
#define SAMEIC_H_

#include "FreeRTOS.h"
#include "samgpio.h"

typedef enum {EIC0 = 0, EIC1, EIC2, EIC3, EIC4, EIC5, EIC6, EIC7, EIC8, EIC9, EIC10, EIC11, EIC12, EIC13, EIC14, EIC15, EIC_NMI, EIC_OFF} eEICChannels_t;
typedef enum {EIC_None = 0, EIC_Rise, EIC_Fall, EIC_Both, EIC_High, EIC_Low} eEICSense_t;

typedef void (*EICCallback_t)(eEICChannels_t channel);

void sam_EICInit(uint8_t clksource);
void sam_EICEnableExtIRQ(eEICChannels_t channel);
void sam_EICDisableExtIRQ(eEICChannels_t channel);
void sam_EICClearIRQ(eEICChannels_t channel);
void sam_EICConfigExt(eEICChannels_t channel, BaseType_t debouce, BaseType_t filter, eEICSense_t sense);
eEICChannels_t sam_getGPIOEICChannel(GPIOPin_t gpio);

void sam_EICSetCallback(eEICChannels_t channel, EICCallback_t cb);

#endif /* SAMEIC_H_ */