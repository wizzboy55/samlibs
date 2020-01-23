/*
 * samdac.h
 *
 * Created: 2020-01-23 10:00:41 AM
 *  Author: charl
 */ 


#ifndef SAMDAC_H_
#define SAMDAC_H_

#include "FreeRTOS.h"

typedef enum {DACRef_VREFAU = 0, DACRef_VDDANA, DACRef_VREFAB, DACRef_INTREF} eDacReference_t;
typedef enum {DACOSR_1x = 0, DACOSR_2x, DACOSR_4x, DACOSR_8x, DACOSR_16x, DACOSR_32x} eDacOversampling_t;

typedef struct {
	eDacReference_t ref;
	BaseType_t differential;
	eDacOversampling_t dac0oversampling;
	eDacOversampling_t dac1oversampling;
} samDacConfig_t;



#endif /* SAMDAC_H_ */