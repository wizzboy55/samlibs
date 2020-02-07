/*
 * phy_generic.h
 *
 * Created: 2020-01-25 10:58:35 AM
 *  Author: charl
 */ 


#ifndef PHY_GENERIC_H_
#define PHY_GENERIC_H_

#include <stdint.h>

#include "FreeRTOS.h"
#include "IEEE802Definitions.h"
#include "IEEEClause22Registers.h"

typedef BaseType_t (*MIIWriteToPHYFunction_t)(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t data, BaseType_t wait);
typedef BaseType_t (*MIIReadFromPHYFunction_t)(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t *data, BaseType_t wait);
typedef BaseType_t (*SERCOMWriteToPhyFunction_t)(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count);
typedef BaseType_t (*SERCOMReadFromPHYFunction_t)(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count);
typedef void (*GetMACSpeedCapabilityFunction_t)(IEEELinkCapabilities_t* capabilities);
typedef BaseType_t (*SetInterfaceSpeedFunction_t)(IEEELinkCapabilities_t selfCapabilities, IEEELinkCapabilities_t connectedCapabilities);
typedef void (*LinkStatusChangeFunction_t)(BaseType_t linkUp);

typedef struct {
	MIIWriteToPHYFunction_t miiWriteToPhy;
	MIIReadFromPHYFunction_t miiReadFromPhy;
	SERCOMWriteToPhyFunction_t sercomWriteToPhy;
	SERCOMReadFromPHYFunction_t sercomReadFromPhy;
	GetMACSpeedCapabilityFunction_t getMACSpeedCapability;
	SetInterfaceSpeedFunction_t setInterfaceSpeed;
	LinkStatusChangeFunction_t linkStatusChange;
	uint8_t phyAddr;
	uint8_t phyClause;
} PhyMaintenanceParameters_t;

void vPHYGenericMaintenanceTask(void *p);
void vPHYGenericInterruptHandler(void);
void vPHYGenericPowerEventHandler(void);

#endif /* PHY_GENERIC_H_ */