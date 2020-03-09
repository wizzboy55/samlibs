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
#define DMX_STARTCODE_INDEX 0

#define DMX_BUFFERING 3

#define DMX_DIRPIN_RX 0
#define DMX_DIRPIN_TX 1
#define DMX_BREAKCOUNT 4
#define DMX_BREAKBYTE 0x00

#define DMX_STARTCODE 0x00
#define RDM_STARTCODE 0xCC

typedef uint16_t RdmChecksum_t;

typedef struct {
	uint16_t manufacturerID;
	uint32_t deviceID;
} __attribute__((packed, aligned(1))) RdmUniqueID_t;

typedef struct {
	union {
		uint8_t dmx[DMX_MAX_SLOTS + sizeof(uint8_t)];
		struct {
			uint8_t startcode;
			uint8_t substartcode;
			uint8_t messageLength;
			RdmUniqueID_t destinationUID;
			RdmUniqueID_t sourceUID;
			uint8_t transationNumber;
			union {
				uint8_t portID;
				uint8_t responseType;
			};
			uint8_t messageCount;
			uint16_t subDevice;
			uint8_t message[];
		} __attribute__((packed, aligned(1))) rdm;
	};
	uint16_t slotCount;
	struct {
		uint8_t used:1;
		uint8_t valid:1;
	} status;
} DmxBuffer_t;

typedef enum {DmxState_Idle, DmxState_Break, DmxState_Mark, DmxState_StartCode, DmxState_Slots, DmxState_Rdm, DmxMaxStates} DmxState_t;
typedef enum {DmxActivity_Idle, DmxActivity_Dmx, DmxActivity_Rdm} DmxActivity_t;

// Callbacks
typedef void (*vDmxNewRxFrame)(DmxBuffer_t*);
typedef void (*vDmxEvent)(void);

typedef struct {
	void* module;
	uint32_t rxpin;
	uint32_t txpin;
	uint32_t pinmux;
	uint32_t dirpin;
	uint32_t ledpin;
	BaseType_t ledinv;
} DmxPortHardware_t;

typedef struct {
	DmxPortHardware_t hw;
	DmxState_t rxState;
	DmxState_t txState;
	uint16_t currentTxSlot;
	uint8_t breakByteCount;
	DmxBuffer_t rxBuffers[DMX_BUFFERING];
	DmxBuffer_t* currentRxBuffer;
	DmxBuffer_t* lastValidRxBuffer;
	DmxBuffer_t* currentTxBuffer;
	vDmxNewRxFrame cb_newRxFrame;
	vDmxNewRxFrame cb_newRdmMessage;
	vDmxEvent cb_byteReceived;
	DmxActivity_t activity;
} DmxPortConfig_t;

BaseType_t xDmxInitSercom(DmxPortConfig_t* config);
void vDmxInterruptHandler(DmxPortConfig_t* config);
BaseType_t xDmxSendFrame(DmxPortConfig_t* config, DmxBuffer_t* frame);
void vDmxSwapRxTxPins(DmxPortConfig_t* config, BaseType_t swap);

#endif /* DMX_H_ */