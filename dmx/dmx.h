/*
 * dmx.h
 *
 * Created: 2019-09-20 5:14:35 PM
 *  Author: charl
 */ 


#ifndef DMX_H_
#define DMX_H_

#include <stdint.h>

#include "FreeRTOS.h"

#include "sercom.h"

#define DMX_MAX_SLOTS 512
#define DMX_STARTCODE_LENGTH 1

#define DMX_BUFFERING 3

#define DMX_DIRPIN_RX 0
#define DMX_DIRPIN_TX 1
#define DMX_BREAKCOUNT 4
#define DMX_BREAKBYTE 0x00

#define DMX_STARTCODE_DMX 0x00
#define DMX_STARTCODE_RDM 0x55

typedef struct {
	union {
		uint8_t data[DMX_MAX_SLOTS + DMX_STARTCODE_LENGTH];
		struct {
			uint8_t startcode;
			uint8_t slots[DMX_MAX_SLOTS];
		} dmx;
	};
	uint16_t slotCount;
	struct {
		uint8_t used:1;
		uint8_t valid:1;
	} status;
} DmxBuffer_t;

typedef enum {DmxState_Idle, DmxState_Break, DmxState_Mark, DmxState_StartCode, DmxState_Slots, DmxState_Rdm, DmxMaxStates} DmxState_t;

// Callbacks
typedef void (*vDmxNewRxFrame)(DmxBuffer_t*);

typedef struct {
	struct {
		void* module;
		uint32_t rxpin;
		uint32_t rxfunc;
		enum SercomPads_e rxpad;
		uint32_t txpin;
		uint32_t txfunc;
		enum SercomPads_e txpad;
	} hw;
	DmxState_t rxState;
	DmxState_t txState;
	uint16_t currentTxSlot;
	uint8_t breakByteCount;
	DmxBuffer_t rxBuffers[DMX_BUFFERING];
	DmxBuffer_t* currentRxBuffer;
	DmxBuffer_t* lastValidRxBuffer;
	DmxBuffer_t* currentTxBuffer;
	vDmxNewRxFrame cb_newRxFrame;
} DmxPortConfig_t;

BaseType_t xDmxInitSercom(DmxPortConfig_t* config);
void vDmxInterruptHandler(DmxPortConfig_t* config);

#endif /* DMX_H_ */