/*
 * ccl.h
 *
 * Created: 2020-03-09 9:46:44 AM
 *  Author: charl
 */ 


#ifndef CCL_H_
#define CCL_H_

#include "FreeRTOS.h"

typedef enum {CCL_MASK = 0, CCL_FEEDBACK, CCL_LINK, CCL_EVENT, CCL_IO, CCL_AC, CCL_TC, CCL_ALTTC, CCL_SERCOM = 0x9, CCL_ALT2TC, CCL_ASYNCEVENT} CCLInputSelect_t;
typedef enum {CCL_DISABLE = 0, CCL_SYNCH, CCL_FILTER} CCLFilterSelect_t;

typedef struct {
	uint32_t in0pin;
	uint32_t in1pin;
	uint32_t in2pin;
	uint32_t outpin;
	uint8_t module;
	uint8_t truthtable;
	CCLInputSelect_t in0sel;
	CCLInputSelect_t in1sel;
	CCLInputSelect_t in2sel;
	CCLFilterSelect_t filtersel;
	BaseType_t edgedetect;
} CCLConfig_t;

void vCCLInit(CCLConfig_t* config);

#endif /* CCL_H_ */