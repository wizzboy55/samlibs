/*
 * ksz9893.c
 *
 * Created: 2020-01-24 9:38:02 AM
 *  Author: charl
 */ 

#include "ksz9893.h"
#include "ksz9893_registers.h"

#include "FreeRTOS.h"
#include "task.h"

#include "debug_interface.h"

BaseType_t xKSZ9893_SetMACInterfaceSpeed(KSZ9893_Parameters_t* params, IEEELinkCapabilities_t selfCapabilities, IEEELinkCapabilities_t macCapabilities) {
	
	KSZ9893_Port3XMIIPortControl0_t port3xmiiPortControl0;
	port3xmiiPortControl0.bit.MACPortTransmitFlowControlEnable = 0;
	port3xmiiPortControl0.bit.MACPortReceiveFlowControlEnable = 0;
	
	KSZ9893_Port3XMIIPortControl1_t port3xmiiPortControl1;
	port3xmiiPortControl1.bit.PortInterfaceTypeSelect = 0b01;
	port3xmiiPortControl1.bit.MII_RMII_RGMII_Modes = 0;
	port3xmiiPortControl1.bit.PortSpeed1000 = 0;
	
	if(selfCapabilities.bit.lcFull100BaseTX && macCapabilities.bit.lcFull100BaseTX) {
		port3xmiiPortControl0.bit.MACPortDuplex = 1;
		port3xmiiPortControl0.bit.MACPortSpeed10_100 = 1;
		debug_printf("KSZ9893: Now in 100BaseTX FD\n");
	} else if(selfCapabilities.bit.lcHalf100BaseTX && macCapabilities.bit.lcHalf100BaseTX) {
		port3xmiiPortControl0.bit.MACPortDuplex = 0;
		port3xmiiPortControl0.bit.MACPortSpeed10_100 = 1;
		debug_printf("KSZ9893: Now in 100BaseTX HD\n");
	} else if(selfCapabilities.bit.lcFull10BaseT && macCapabilities.bit.lcFull10BaseT) {
		port3xmiiPortControl0.bit.MACPortDuplex = 1;
		port3xmiiPortControl0.bit.MACPortSpeed10_100 = 0;
		debug_printf("KSZ9893: Now in 10BaseT FD\n");
	} else if(selfCapabilities.bit.lcHalf10BaseT && macCapabilities.bit.lcHalf10BaseT) {
		port3xmiiPortControl0.bit.MACPortDuplex = 0;
		port3xmiiPortControl0.bit.MACPortSpeed10_100 = 0;
		debug_printf("KSZ9893: Now in 10BaseT HD\n");
	} else {
		debug_printf("KSZ9893: No compatible speed\n");
		return pdFAIL;
	}

	params->sercomWriteToPhy(params->phyAddr, KSZ9893_Port3XMIIPortControl0, (uint8_t*)&port3xmiiPortControl0, sizeof(KSZ9893_Port3XMIIPortControl0_t));
	params->sercomWriteToPhy(params->phyAddr, KSZ9893_Port3XMIIPortControl1, (uint8_t*)&port3xmiiPortControl1, sizeof(KSZ9893_Port3XMIIPortControl1_t));

	return pdPASS;
}

void vKSZ9893Setup(KSZ9893_Parameters_t* params) {
	KSZ9893_PMEPinControl_t pmePinControl;
	pmePinControl.reg = 0;
	pmePinControl.bit.PMEPinOutputEnable = 1;
	pmePinControl.bit.PMEPinOutputPolarity = 0;
	
	params->sercomWriteToPhy(params->phyAddr, KSZ9893_PMEPinControl, (uint8_t*)&pmePinControl, sizeof(pmePinControl));
	
	KSZ9893_GlobalPortInterruptMask_t globalPortInterruptMask;
	globalPortInterruptMask.reg = 0;
	globalPortInterruptMask.bit.Port1InterruptMask = 0;
	globalPortInterruptMask.bit.Port2InterruptMask = 0;
	globalPortInterruptMask.bit.Port3InterruptMask = 0;
	
	params->sercomWriteToPhy(params->phyAddr, KSZ9893_GlobalPortInterruptMask, (uint8_t*)&globalPortInterruptMask, sizeof(globalPortInterruptMask));
	
	KSZ9893_IODriveStrength_t ioDriveStrength;
	ioDriveStrength.bit.HighSpeedDriveStrength = 0x7;
	ioDriveStrength.bit.LowSpeedDriveStrength = 0x7;
	
	params->sercomWriteToPhy(params->phyAddr, KSZ9893_IODriveStrength, (uint8_t*)&ioDriveStrength, sizeof(KSZ9893_IODriveStrength_t));
	
	IEEELinkCapabilities_t macLinkCapabilities;
	IEEELinkCapabilities_t ksz9893LinkCapabilities;
	ksz9893LinkCapabilities.bit.lcFull100BaseTX = 1;
	ksz9893LinkCapabilities.bit.lcHalf100BaseTX = 1;
	ksz9893LinkCapabilities.bit.lcFull10BaseT = 1;
	ksz9893LinkCapabilities.bit.lcHalf10BaseT = 1;
	
	params->getMACSpeedCapability(&macLinkCapabilities);
	params->setInterfaceSpeed(macLinkCapabilities, ksz9893LinkCapabilities);
	xKSZ9893_SetMACInterfaceSpeed(params, ksz9893LinkCapabilities, macLinkCapabilities);
}

void vKSZ9893MaintenanceTask(void *p) {
	
	KSZ9893_Parameters_t* params = (KSZ9893_Parameters_t*)p;
	
	uint8_t ksz9893_guid[5];
	params->sercomReadFromPhy(params->phyAddr, KSZ9893_GlobalChipID0, &ksz9893_guid[1], 4);
	params->sercomReadFromPhy(params->phyAddr, KSZ9893_GlobalChipID4, &ksz9893_guid[0], 1);
	debug_printf("KSZ9893 GUID: 0x%02X%02X%02X%02X%02X\n", ksz9893_guid[0], ksz9893_guid[1], ksz9893_guid[2], ksz9893_guid[3], ksz9893_guid[4]);
	
	vKSZ9893Setup(params);
	
	BaseType_t port1isUp = pdFALSE;
	BaseType_t port2isUp = pdFALSE;
	BaseType_t port1wasUp = pdFALSE;
	BaseType_t port2wasUp = pdFALSE;
	
	for(;;) {
		
		KSZ9893_PortNPHYBasicStatus_t port1PHYBasicStatus;
		params->sercomReadFromPhy(params->phyAddr, KSZ9893_Port1Offset | KSZ9893_PortNPHYBasicStatus, (uint8_t*)&port1PHYBasicStatus, sizeof(KSZ9893_PortNPHYBasicStatus_t));
		
		KSZ9893_PortNPHYBasicStatus_t port2PHYBasicStatus;
		params->sercomReadFromPhy(params->phyAddr, KSZ9893_Port2Offset | KSZ9893_PortNPHYBasicStatus, (uint8_t*)&port2PHYBasicStatus, sizeof(KSZ9893_PortNPHYBasicStatus_t));
		
		port1isUp = port1PHYBasicStatus.bit.LinkStatus;
		port2isUp = port2PHYBasicStatus.bit.LinkStatus;
		
		if(port1isUp != port1wasUp) {
			if(port1isUp) {
				debug_printf("Port 1 is up.\n");
				params->linkStatusChange(pdTRUE);
			} else {
				debug_printf("Port 1 is down.\n");
				if(port2isUp == pdFALSE) {
					params->linkStatusChange(pdFALSE);
				}
			}
			port1wasUp = port1isUp;
		}
		
		if(port2isUp != port2wasUp) {
			if(port2isUp) {
				debug_printf("Port 2 is up.\n");
				params->linkStatusChange(pdTRUE);
			} else {
				debug_printf("Port 2 is down.\n");
				if(port1isUp == pdFALSE) {
					params->linkStatusChange(pdFALSE);
				}
			}
			port2wasUp = port2isUp;
		}
		
		vTaskDelay(1000 / portTICK_PERIOD_MS);	
	}
}

void vKSZ9893InterruptHandler(void) {
	
}

void vKSZ9893PowerEventHandler(void) {
	
}