/*
 * phy_generic.c
 *
 * Created: 2020-01-25 10:58:44 AM
 *  Author: charl
 */ 

#include "phy_generic.h"

#include "task.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"

#include "debug_interface.h"

void vPHYGenericMaintenanceTask(void *p) {
	
	PhyMaintenanceParameters_t* params = (PhyMaintenanceParameters_t*)p;
	
	Clause22_BasicStatus_t phyBasicStatus;
	Clause22_AutoNegotiationLinkPartner_t connectedLinkPartnerAbility;
	IEEELinkCapabilities_t macLinkCapabilities;
	IEEELinkCapabilities_t phySelfLinkCapabilities;
	IEEELinkCapabilities_t connectedLinkCapabilities;
	Clause22_AutoNegotiationAdvertisement_t phyAutoNegAdvertisement;
	Clause22_BasicControl_t phyBasicControl;
	Clause22_AutoNegotiationExpansion_t connectAutoNegExpansion;

	params->getMACSpeedCapability(&macLinkCapabilities);

	for(;;) {
		
		vTaskDelay(pdMS_TO_TICKS(100));

		phyAutoNegAdvertisement.reg = 0x00;
		phyAutoNegAdvertisement.bit.b100BaseTXFullDuplex = macLinkCapabilities.bit.lcFull100BaseTX;
		phyAutoNegAdvertisement.bit.b100BaseTXHalfDuplex = macLinkCapabilities.bit.lcHalf100BaseTX;
		phyAutoNegAdvertisement.bit.b10BaseTFullDuplex = macLinkCapabilities.bit.lcFull10BaseT;
		phyAutoNegAdvertisement.bit.b10BaseTHalfDuplex = macLinkCapabilities.bit.lcHalf10BaseT;
		phyAutoNegAdvertisement.bit.SelectorField = Clause22_SelectorField_IEEE802_3;

		if(params->miiWriteToPhy(params->phyAddr, params->phyClause, Clause22_AutoNegotiationAdvertisement, phyAutoNegAdvertisement.reg, pdFALSE) == pdFAIL) {
			continue;
		}

		phyBasicControl.reg = 0x00;
		phyBasicControl.bit.AutoNegotiationEnable = 1;
		phyBasicControl.bit.RestartAutoNegotiation = 0;
		phyBasicControl.bit.DuplexMode = 1;
		phyBasicControl.bit.SpeedSelect = 1;

		if(params->miiWriteToPhy(params->phyAddr, params->phyClause, Clause22_BasicControl, phyBasicControl.reg, pdFALSE) == pdFAIL) {
			continue;
		}

		for(;;) {
			if(params->miiReadFromPhy(params->phyAddr, params->phyClause, Clause22_BasicStatus, &phyBasicStatus.reg, pdFALSE) == pdFAIL) {
				break;
			}

			phySelfLinkCapabilities.reg = 0x00;

			phySelfLinkCapabilities.bit.lcHalf100BaseT4 = phyBasicStatus.bit.b100BaseT4;
			phySelfLinkCapabilities.bit.lcFull100BaseTX = phyBasicStatus.bit.b100BaseTXFullDuplex;
			phySelfLinkCapabilities.bit.lcHalf100BaseTX = phyBasicStatus.bit.b100BaseTXHalfDuplex;
			phySelfLinkCapabilities.bit.lcFull10BaseT = phyBasicStatus.bit.b10BaseTFullDuplex;
			phySelfLinkCapabilities.bit.lcHalf10BaseT = phyBasicStatus.bit.b10BaseTHalfDuplex;

			if(phyBasicStatus.bit.AutoNegotiationComplete) {
				if(params->miiReadFromPhy(params->phyAddr, params->phyClause, Clause22_AutoNegotiationLinkPartner, &connectedLinkPartnerAbility.reg, pdFALSE) == pdFAIL) {
					break;
				}

				if(connectedLinkPartnerAbility.bit.RemoteFault) {
					DEBUG_printf( ("Remote Fault detected.\n") );
				}

				if(params->miiReadFromPhy(params->phyAddr, params->phyClause, Clause22_AutoNegotiationExpansion, &connectAutoNegExpansion.reg, pdFALSE) == pdFAIL) {
					break;
				}
				
				if(connectedLinkPartnerAbility.bit.Acknowledge) {
					connectedLinkCapabilities.reg = 0x00;
					connectedLinkCapabilities.bit.lcHalf100BaseT4 = connectedLinkPartnerAbility.bit.b100BaseT4;
					connectedLinkCapabilities.bit.lcFull100BaseTX = connectedLinkPartnerAbility.bit.b100BaseTXFullDuplex;
					connectedLinkCapabilities.bit.lcHalf100BaseTX = connectedLinkPartnerAbility.bit.b100BaseTXHalfDuplex;
					connectedLinkCapabilities.bit.lcFull10BaseT = connectedLinkPartnerAbility.bit.b10BaseTFullDuplex;
					connectedLinkCapabilities.bit.lcHalf10BaseT = connectedLinkPartnerAbility.bit.b10BaseTHalfDuplex;

					if(phyBasicStatus.bit.LinkStatus) {
						// Link is UP
						if(params->setInterfaceSpeed(phySelfLinkCapabilities, connectedLinkCapabilities) == pdPASS) {
							params->linkStatusChange(pdTRUE);
						} else {
							DEBUG_printf( ("ERROR: Network Incompatible Link Speeds.\n") );
							params->linkStatusChange(pdFALSE);
						}
					} else {
						params->linkStatusChange(pdFALSE);
					}
				}
			}
		}
	}
}

void vPHYGenericInterruptHandler(void) {
	
}

void vPHYGenericPowerEventHandler(void) {
	
}