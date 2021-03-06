/*
 * samx5xmac.h
 *
 * Created: 2019-07-30 2:07:24 PM
 *  Author: Wizz
 */ 


#ifndef SAMX5XMAC_H_
#define SAMX5XMAC_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "list.h"
#include "FreeRTOS_IP.h"
#include "i2c.h"
#include "saminterrupt.h"
#include "samgpio.h"

#include "IEEE802Definitions.h"

#define ETH_BUFFER_SIZE ( ipTOTAL_ETHERNET_FRAME_SIZE + ipBUFFER_PADDING )
#define ETH_BUFFER_SIZE_ROUNDED_UP (( ETH_BUFFER_SIZE + 7 ) & ~0x07UL )

typedef enum PhyClause {PHY_CLAUSE_22 = 22, PHY_CLAUSE_45 = 45} eSAMx5xMAC_PhyClause_t;

typedef struct {
	GPIOPin_t gpio_mdc;
	GPIOPin_t gpio_mdio;
	PinMux_t pinmux_mdc;
	PinMux_t pinmux_mdio;
	GPIOPin_t gpio_reset;
	GPIOPin_t gpio_interrupt;
	GPIOPin_t gpio_powerEvent;
	GPIOPin_t gpio_crsdv;
	uint8_t clksource;
	BaseType_t useMII;
	eSAMx5xMAC_PhyClause_t phyclause;
	IEEELinkCapabilities_t linkCapabilities;
	i2cConfig_t *i2cConfig;
	TaskFunction_t phyMaintenanceTask;
	void* phyMaintenanceParameters;
	InterruptHandler_t phyInterruptHandler;
	InterruptHandler_t phyPowerEventHandler;
} SAMx5xMAC_HwConfig_t;

typedef struct {
	union {
		uint8_t* address;
		struct {
			uint32_t Ownership:1;
			uint32_t Wrap:1;
			uint32_t :29;
		} flags;
	} Word0;
	union {
		struct {
			uint32_t FrameLength:13;
			uint32_t ChecksumGood:1;
			uint32_t StartOfFrame:1;
			uint32_t EndOfFrame:1;
			uint32_t CanonicalFormatIndicator:1;
			uint32_t VLANPriority:3;
			uint32_t PriorityTagDetected:1;
			uint32_t VLANTagDetected:1;
			uint32_t TypeIDMatch_ChecksumCorrect:2;
			uint32_t TypeIDFound_SNAPEncoded:1;
			uint32_t SpecificAddressMatch:2;
			uint32_t SpecificAddressMatchFound:1;
			uint32_t :1;
			uint32_t UnicastHashMatch:1;
			uint32_t MulticastHashMatch:1;
			uint32_t BroadcastDetected:1;
		} bit;
		uint32_t reg;
	} receiveStatus;
} SAMx5xMAC_ReceiveBuffer_t;

#define SAMx5xMAC_ReceiveAddressMask 0xFFFFFFFC

typedef struct {
	uint8_t* address;
	union {
		struct {
			uint32_t BufferLength:14;
			uint32_t :1;
			uint32_t LastBuffer:1;
			uint32_t CRCIncluded:1;
			uint32_t :3;
			uint32_t ChecksumErrors:3;
			uint32_t :3;
			uint32_t LateCollision:1;
			uint32_t AHBError:1;
			uint32_t :1;
			uint32_t RetryLimitExceeded:1;
			uint32_t Wrap:1;
			uint32_t Used:1;
		} bit;
		uint32_t reg;
	} Word1;
} SAMx5xMAC_TransmitBuffer_t;

BaseType_t xSAMx5xMAC_Initialize(SAMx5xMAC_HwConfig_t* pConfig);
BaseType_t xSAMx5xMAC_OutputFrame(NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t xReleaseAfterSend);
void vSAMx5xMAC_SetMACAddress(uint8_t macAddress[]);
void xSAMx5xMAC_LinkStatusChanged(BaseType_t linkUp);

BaseType_t xSAMx5xMAC_MIIWriteToPHY(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t data, BaseType_t wait);
BaseType_t xSAMx5xMAC_MIIReadFromPHY(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t *data, BaseType_t wait);
BaseType_t xSAMx5xMAC_SetInterfaceSpeed(IEEELinkCapabilities_t selfCapabilities, IEEELinkCapabilities_t connectedCapabilities);
void xSAMx5xMAC_LinkStatusChanged(BaseType_t linkUp);
void xSAMx5xMAC_GetMACSpeedCapability(IEEELinkCapabilities_t* capabilities);

BaseType_t xSAMx5xMAC_I2CWriteToPHY(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count);
BaseType_t xSAMx5xMAC_I2CReadFromPHY(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count);

#endif /* SAMX5XMAC_H_ */