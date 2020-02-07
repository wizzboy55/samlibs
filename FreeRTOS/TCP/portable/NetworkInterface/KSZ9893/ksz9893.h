/*
 * ksz9893.h
 *
 * Created: 2020-01-24 9:37:55 AM
 *  Author: charl
 */ 


#ifndef KSZ9893_H_
#define KSZ9893_H_

#include "ksz9893.h"

#include "FreeRTOS.h"

#include "phy_generic.h"

typedef struct {
	SERCOMWriteToPhyFunction_t sercomWriteToPhy;
	SERCOMReadFromPHYFunction_t sercomReadFromPhy;
	GetMACSpeedCapabilityFunction_t getMACSpeedCapability;
	SetInterfaceSpeedFunction_t setInterfaceSpeed;
	LinkStatusChangeFunction_t linkStatusChange;
	uint8_t phyAddr;
	uint8_t phyClause;
} KSZ9893_Parameters_t;

void vKSZ9893MaintenanceTask(void *p);
void vKSZ9893InterruptHandler(void);
void vKSZ9893PowerEventHandler(void);

#endif /* KSZ9893_H_ */