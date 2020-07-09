/*
 * samx5xmac.c
 *
 * Created: 2019-07-30 2:07:33 PM
 *  Author: Wizz
 */ 

#include "samx5xmac.h"

#include "hal_gpio.h"

#include "samclk.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "IEEEClause22Registers.h"
#include "HardwareDescriptor.h"
#include "sameic.h"
#include "utilities.h"

#include "debug_interface.h"

#define ipconfigNUM_RX_BUFFERS 5
#define ipconfigNUM_TX_BUFFERS 5

SAMx5xMAC_ReceiveBuffer_t ethReceiveBuffers[ipconfigNUM_RX_BUFFERS];
SAMx5xMAC_TransmitBuffer_t ethTransmitBuffers[ipconfigNUM_TX_BUFFERS];
TaskHandle_t samx5xmac_deferredrxhandler = NULL;
TaskHandle_t samx5xmac_phymaintenancehandler = NULL;
TaskHandle_t initTask = NULL;
SAMx5xMAC_HwConfig_t* pMacConfig = NULL;
BaseType_t isInit = pdFALSE;
BaseType_t LinkIsUp = pdFALSE;

#if ((ipconfigBUFFER_PADDING % 4) != 0)
	#error "ipBUFFER_PADDING must be a multiple of 4 for proper padding"
#endif

BaseType_t xSAMx5xMAC_MIIWriteToPHY(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t data, BaseType_t wait) {
	GMAC->NCR.bit.MPE = 1;
		
	GMAC->IER.bit.MFS = 1;

	GMAC->MAN.reg = GMAC_MAN_WTN(0b10) | GMAC_MAN_PHYA(phyaddr) | GMAC_MAN_REGA(reg) | (((clause == PHY_CLAUSE_22) & 0x1) << GMAC_MAN_CLTTO_Pos) | GMAC_MAN_OP(0b01) | GMAC_MAN_DATA(data);

	if(wait == pdTRUE) {
		for(BaseType_t tries = 10; tries > 0; tries--) {
			vTaskDelay(pdMS_TO_TICKS(5));
			if(GMAC->NSR.bit.IDLE) {
				return pdPASS;
			}
		}
	} else {
		if(xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(500)) == pdTRUE) {
			return pdPASS;
		}
	}

	DEBUG_printf( ("ERROR: Unable to write PHY.\n") );
	return pdFAIL;
}

BaseType_t xSAMx5xMAC_MIIReadFromPHY(uint8_t phyaddr, uint8_t clause, uint8_t reg, uint16_t *data, BaseType_t wait) {
	GMAC->NCR.bit.MPE = 1;
		
	GMAC->IER.bit.MFS = 1;

	GMAC->MAN.reg = GMAC_MAN_WTN(0b10) | GMAC_MAN_PHYA(phyaddr) | GMAC_MAN_REGA(reg) | (((clause == PHY_CLAUSE_22) & 0x1) << GMAC_MAN_CLTTO_Pos) | GMAC_MAN_OP(0b10);
		
	if(wait == pdTRUE) {
		for(BaseType_t tries = 10; tries > 0; tries--) {
			vTaskDelay(pdMS_TO_TICKS(5));
			if(GMAC->NSR.bit.IDLE) {
				*data = GMAC->MAN.bit.DATA;
				return pdPASS;
			}
		}
	} else {
		if(xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(500)) == pdTRUE) {
			*data = GMAC->MAN.bit.DATA;
			return pdPASS;
		}
	}


	DEBUG_printf( ("ERROR: Unable to read from PHY.\n") );
	return pdFAIL;
}

BaseType_t xSAMx5xMAC_I2CWriteToPHY(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count) {
	uint8_t ret = i2c_master_writeBytes16_BE(pMacConfig->i2cConfig, phyaddr, reg, data, count);
	UNUSED(ret);
	return pdPASS;
}

BaseType_t xSAMx5xMAC_I2CReadFromPHY(uint8_t phyaddr, uint16_t reg, uint8_t *data, uint16_t count) {
	uint8_t ret = i2c_master_writeAddr16_BE(pMacConfig->i2cConfig, phyaddr, reg);
	ret = i2c_master_readBytes_BE(pMacConfig->i2cConfig, phyaddr, data, count);
	if(ret > 0) {
		return pdPASS;
	}
	return pdFAIL;
}

SAMx5xMAC_ReceiveBuffer_t* pxSAMx5xMAC_GetNextReceiveBuffer(void) {
	for(BaseType_t x = 0; x < ipconfigNUM_RX_BUFFERS; x++) {
		if(ethReceiveBuffers[x].Word0.flags.Ownership == 1) {
			return &(ethReceiveBuffers[x]);
		}
	}

	return NULL;
}

void vSAMx5xMAC_DeferredRxInterruptHandlerTask(void *p) {
	
	UNUSED(p);
	
	GMAC->IER.bit.RCOMP = 1;
	
	for(;;) {

		// Get received frame descriptor
		SAMx5xMAC_ReceiveBuffer_t* pxReceiveBuffer = pxSAMx5xMAC_GetNextReceiveBuffer();

		if(pxReceiveBuffer == NULL) {
			// No frame received.
			xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(250));
			continue;
		}

		// Double check
		if(pxReceiveBuffer == NULL) {
			continue;
		}

		NetworkBufferDescriptor_t* pxDescriptor = pxGetNetworkBufferWithDescriptor(ETH_BUFFER_SIZE_ROUNDED_UP, 0);

		uint8_t* pucTemp = pxDescriptor->pucEthernetBuffer;
		uint8_t temp_wrap = pxReceiveBuffer->Word0.flags.Wrap;

		pxDescriptor->pucEthernetBuffer = (uint8_t*)(((uint32_t)pxReceiveBuffer->Word0.address) & SAMx5xMAC_ReceiveAddressMask);
		pxDescriptor->xDataLength = pxReceiveBuffer->receiveStatus.bit.FrameLength;

		pxReceiveBuffer->Word0.address = (uint8_t*)pucTemp;
		pxReceiveBuffer->Word0.flags.Wrap = temp_wrap;

		*((NetworkBufferDescriptor_t **) (pxDescriptor->pucEthernetBuffer - ipBUFFER_PADDING) ) = pxDescriptor;

		if(eConsiderFrameForProcessing(pxDescriptor->pucEthernetBuffer) == eProcessBuffer) {
			IPStackEvent_t xRxEvent;
			xRxEvent.eEventType = eNetworkRxEvent;
			xRxEvent.pvData = (void*) pxDescriptor;

			if(xSendEventStructToIPTask(&xRxEvent, 0) == pdFALSE) {
				vReleaseNetworkBufferAndDescriptor(pxDescriptor);
				iptraceETHERNET_RX_EVENT_LOST();
			} else {
				iptraceNETWORK_INTERFACE_RECEIVE();
			}
		} else {
			vReleaseNetworkBufferAndDescriptor(pxDescriptor);
		}
	}
}

BaseType_t xSAMx5xMAC_SetInterfaceSpeed(IEEELinkCapabilities_t selfCapabilities, IEEELinkCapabilities_t connectedCapabilities) {
	if(selfCapabilities.bit.lcFull100BaseTX && connectedCapabilities.bit.lcFull100BaseTX) {
		GMAC->NCFGR.bit.FD = 1;
		GMAC->NCFGR.bit.SPD = 1;
		debug_printf("GMAC: Now in 100BaseTX FD\n");
	} else if(selfCapabilities.bit.lcHalf100BaseTX && connectedCapabilities.bit.lcHalf100BaseTX) {
		GMAC->NCFGR.bit.FD = 0;
		GMAC->NCFGR.bit.SPD = 1;
		debug_printf("GMAC: Now in 100BaseTX HD\n");
	} else if(selfCapabilities.bit.lcFull10BaseT && connectedCapabilities.bit.lcFull10BaseT) {
		GMAC->NCFGR.bit.FD = 1;
		GMAC->NCFGR.bit.SPD = 0;
		debug_printf("GMAC: Now in 10BaseT FD\n");
	} else if(selfCapabilities.bit.lcHalf10BaseT && connectedCapabilities.bit.lcHalf10BaseT) {
		GMAC->NCFGR.bit.FD = 0;
		GMAC->NCFGR.bit.SPD = 0;
		debug_printf("GMAC: Now in 10BaseT HD\n");
	} else {
		debug_printf("GMAC: No compatible speed\n");
		return pdFAIL;
	}

	return pdPASS;
}

void xSAMx5xMAC_GetMACSpeedCapability(IEEELinkCapabilities_t* capabilities) {
	capabilities->reg = pMacConfig->linkCapabilities.reg;
}

void xSAMx5xMAC_LinkStatusChanged(BaseType_t linkUp) {
	if(linkUp == pdTRUE) {
		GMAC->NCR.bit.RXEN = 1;
		LinkIsUp = pdTRUE;
		if(initTask != NULL) {
			xTaskNotifyGive(initTask);
		}
	} else {
		GMAC->NCR.bit.RXEN = 0;
		LinkIsUp = pdFALSE;
	}
	
	if(linkUp == pdFALSE && LinkIsUp == pdTRUE) {
		IPStackEvent_t networkDown;
		networkDown.eEventType = eNetworkDownEvent;
		networkDown.pvData = NULL;
		xSendEventStructToIPTask(&networkDown, 0);
		GMAC->NCR.bit.RXEN = 0;
		LinkIsUp = pdFALSE;
	}
}

void vSAMx5xMAC_TransmitInterrupt(GMAC_ISR_Type isr, GMAC_TSR_Type tsr) {
	
	GMAC->IDR.bit.TCOMP = 1;
	GMAC->IDR.bit.TUR = 1;
	GMAC->IDR.bit.TFC = 1;
	GMAC->IDR.bit.RLEX = 1;

	if(isr.bit.TFC) {
		DEBUG_printf( ("GMAC: AHB Error while sending packet\n") );
	}
	if(isr.bit.RLEX || tsr.bit.RLE) {
		DEBUG_printf( ("GMAC: Retry limit exceed during transmit\n") );
	}
	if(tsr.bit.COL) {
		DEBUG_printf( ("GMAC: Collision occurred during transmit\n") );
	}

	GMAC->NCR.bit.TXEN = 0;
}

void GMAC_Handler(void) {
	GMAC_ISR_Type isr = GMAC->ISR;

	if(isr.bit.TCOMP || isr.bit.TUR || isr.bit.TFC || isr.bit.RLEX) {
		GMAC_TSR_Type tsr = GMAC->TSR;
		GMAC->TSR.bit.TXCOMP = 1; // Clear bit
		vSAMx5xMAC_TransmitInterrupt(isr, tsr);
	}
	if(isr.bit.RCOMP) {
		if(samx5xmac_deferredrxhandler != NULL) {
			vTaskNotifyGiveFromISR(samx5xmac_deferredrxhandler, NULL);
		}
	}
	if(isr.bit.MFS) {
		if(samx5xmac_phymaintenancehandler != NULL) {
			vTaskNotifyGiveFromISR(samx5xmac_phymaintenancehandler, NULL);
		}
	}
}

void vSAMx5xMAC_ResetPHY(void) {
	gpio_set_pin_level(pMacConfig->gpio_reset, 0);
	vTaskDelay(pdMS_TO_TICKS(100));
	gpio_set_pin_level(pMacConfig->gpio_reset, 1);
}

BaseType_t xSAMx5xMAC_HardwareInit(SAMx5xMAC_HwConfig_t* pMacConfig) {

	if(pMacConfig->i2cConfig != NULL) {
		i2c_master_initIF(pMacConfig->i2cConfig);
	} else {
		// MDIO
		samgpio_setPinFunction(pMacConfig->gpio_mdio, samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(pMacConfig->gpio_mdc, samgpio_getModulePinmux(GMAC));
	}

	#ifdef SAME54
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 12), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 13), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 14), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 15), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(pMacConfig->gpio_crsdv, samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 17), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 18), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 19), samgpio_getModulePinmux(GMAC));
	GMAC->UR.reg = 0; // RMII
	if(pMacConfig->useMII == pdTRUE) {
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 14), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 15), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 16), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 17), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 18), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 19), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 20), samgpio_getModulePinmux(GMAC));
		samgpio_setPinFunction(GPIO(GPIO_PORTC, 21), samgpio_getModulePinmux(GMAC));
		GMAC->UR.reg = 1; // MII
	}
	#elif defined SAME53
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 12), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 13), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 14), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 15), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(pMacConfig->gpio_crsdv, samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 17), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 18), samgpio_getModulePinmux(GMAC));
	samgpio_setPinFunction(GPIO(GPIO_PORTA, 19), samgpio_getModulePinmux(GMAC));
	GMAC->UR.bit.MII = 0; // RMII
	#else
	#error "MCU Family not supported."
	return pdFAIL;
	#endif

	gpio_set_pin_level(pMacConfig->gpio_reset, 1);
	gpio_set_pin_direction(pMacConfig->gpio_reset, GPIO_DIRECTION_OUT);

	if(pMacConfig->gpio_interrupt != GPIO_NONE) {
		samgpio_setPinDirection(pMacConfig->gpio_interrupt, GPIO_DIRECTION_IN);
		samgpio_setPinPullMode(pMacConfig->gpio_interrupt, GPIO_PULL_UP);
		samgpio_setPinFunction(pMacConfig->gpio_interrupt, samgpio_getModulePinmux(EIC));
		sam_EICInit(0);
		sam_EICEnableExtIRQ(sam_getGPIOEICChannel(pMacConfig->gpio_interrupt));
	}
	
	if(pMacConfig->gpio_powerEvent != GPIO_NONE) {
		samgpio_setPinDirection(pMacConfig->gpio_powerEvent, GPIO_DIRECTION_IN);
		samgpio_setPinPullMode(pMacConfig->gpio_powerEvent, GPIO_PULL_UP);
		samgpio_setPinFunction(pMacConfig->gpio_powerEvent, samgpio_getModulePinmux(EIC));
		sam_EICInit(0);
		sam_EICEnableExtIRQ(sam_getGPIOEICChannel(pMacConfig->gpio_powerEvent));
	}

	return pdPASS;
}

BaseType_t xSAMx5xMAC_InitBuffers(void) {
	memset(ethTransmitBuffers, 0, sizeof(ethTransmitBuffers));
	for(BaseType_t x = 0; x < ipconfigNUM_TX_BUFFERS; x++) {
		ethTransmitBuffers[x].Word1.reg = 0x00;
		ethTransmitBuffers[x].address = NULL;
		ethTransmitBuffers[x].Word1.bit.Used = 1;
	}
	ethTransmitBuffers[ipconfigNUM_TX_BUFFERS - 1].Word1.bit.Wrap = 1;
	GMAC->TBQB.reg = (uint32_t) &ethTransmitBuffers[0];

	memset(ethReceiveBuffers, 0, sizeof(ethReceiveBuffers));
	size_t uRequestedBytes = ETH_BUFFER_SIZE_ROUNDED_UP;
	for(BaseType_t x = 0; x < ipconfigNUM_RX_BUFFERS; x++) {
		ethReceiveBuffers[x].Word0.address = (uint8_t*)pucGetNetworkBuffer(&uRequestedBytes);
		ethReceiveBuffers[x].Word0.address = (uint8_t*)((uint32_t)ethReceiveBuffers[x].Word0.address & SAMx5xMAC_ReceiveAddressMask);
		ethReceiveBuffers[x].receiveStatus.reg = 0x00;
		ethReceiveBuffers[x].Word0.flags.Ownership = 0;
	}
	ethReceiveBuffers[ipconfigNUM_RX_BUFFERS - 1].Word0.flags.Wrap = 1;
	GMAC->RBQB.reg = (uint32_t) &ethReceiveBuffers[0];

	return pdPASS;
}

BaseType_t xSAMx5xMAC_Initialize(SAMx5xMAC_HwConfig_t* pConfig) {

	BaseType_t xReturn = pdPASS;

	initTask = xTaskGetCurrentTaskHandle();

	if(isInit != pdTRUE) {
		if(pConfig == NULL) {
			DEBUG_printf( ("ERROR: MAC Configuration NULL.\n") );
			return pdFAIL;
		}

		pMacConfig = pConfig;

		samclk_enable_peripheral_clock(GMAC);
		samclk_enable_gclk_channel(GMAC, pMacConfig->clksource);

		GMAC->NCR.reg = GMAC_NCR_CLRSTAT;

		xSAMx5xMAC_HardwareInit(pMacConfig);

		vSAMx5xMAC_ResetPHY();

		// Registers
		GMAC->NCFGR.reg = GMAC_NCFGR_IRXER | GMAC_NCFGR_RXCOEN | GMAC_NCFGR_CLK(5) | GMAC_NCFGR_PEN | GMAC_NCFGR_CAF;
		GMAC->DCFGR.reg = GMAC_DCFGR_DRBS(ETH_BUFFER_SIZE_ROUNDED_UP / 64) | GMAC_DCFGR_TXPBMS | GMAC_DCFGR_RXBMS(3) | GMAC_DCFGR_FBLDO(4);

		xSAMx5xMAC_InitBuffers();

		xReturn &= xTaskCreate(vSAMx5xMAC_DeferredRxInterruptHandlerTask, "mac_rx", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, &samx5xmac_deferredrxhandler);
		
		if(pConfig->phyMaintenanceTask != NULL) {
			xReturn &= xTaskCreate(pConfig->phyMaintenanceTask, "mac_phy", configMINIMAL_STACK_SIZE*4, pConfig->phyMaintenanceParameters, configMAX_PRIORITIES - 2, &samx5xmac_phymaintenancehandler);
		}

		NVIC_EnableIRQ(GMAC_IRQn);

		isInit = pdTRUE;
	}

	if(xReturn == pdPASS) {
		// Wait for LinkUp
		ulTaskNotifyTake(0x00, portMAX_DELAY);
		if(LinkIsUp == pdTRUE) {
			return pdPASS;
		}
	}

	return pdFAIL;
}

SAMx5xMAC_TransmitBuffer_t* pxSAMx5xMAC_GetNextTransmitBuffer(void) {
	for(BaseType_t x = 0; x < ipconfigNUM_TX_BUFFERS; x++) {
		if(ethTransmitBuffers[x].Word1.bit.Used == 1) {
			return &ethTransmitBuffers[x];
		}
	}

	return NULL;
}

BaseType_t xSAMx5xMAC_OutputFrame(NetworkBufferDescriptor_t * const pxDescriptor, BaseType_t xReleaseAfterSend) {
	SAMx5xMAC_TransmitBuffer_t *pxTransmitBuffer = pxSAMx5xMAC_GetNextTransmitBuffer();

	if(pxTransmitBuffer == NULL) {
		FreeRTOS_debug_printf( ("xNetworkInterfaceOutput: No more TX buffers available\n") );
		if(xReleaseAfterSend != pdFALSE) {
			vReleaseNetworkBufferAndDescriptor(pxDescriptor);
		}
		return pdFAIL;
	}

	uint8_t* previousBuffer = pxTransmitBuffer->address;

	uint8_t wrap = pxTransmitBuffer->Word1.bit.Wrap;

	SAMx5xMAC_TransmitBuffer_t newBuffer;

	newBuffer.address = pxDescriptor->pucEthernetBuffer;
	newBuffer.Word1.reg = 0x00;
	newBuffer.Word1.bit.BufferLength = pxDescriptor->xDataLength;
	newBuffer.Word1.bit.LastBuffer = 1;
	newBuffer.Word1.bit.Used = 0;
	newBuffer.Word1.bit.Wrap = wrap;

	GMAC->NCR.bit.THALT = 1;

	*pxTransmitBuffer = newBuffer;

	GMAC->NCR.bit.TXEN = 1;

	GMAC->IER.bit.TCOMP = 1;
	GMAC->IER.bit.TUR = 1;
	GMAC->IER.bit.TFC = 1;
	GMAC->IER.bit.RLEX = 1;

	GMAC->NCR.bit.TSTART = 1;

	iptraceNETWORK_INTERFACE_TRANSMIT();

	if(xReleaseAfterSend != pdFALSE) {
		pxDescriptor->pucEthernetBuffer = NULL;
		vReleaseNetworkBufferAndDescriptor(pxDescriptor);
	}

	if(previousBuffer != NULL) {
		vReleaseNetworkBuffer(previousBuffer);
	}

	return pdPASS;
}

void vSAMx5xMAC_SetMACAddress(uint8_t macAddress[]) {
	GMAC->Sa[0].SAB.reg = macAddress[0] || (macAddress[1] << 8) || (macAddress[2] << 16) || (macAddress[3] << 24);
	GMAC->Sa[0].SAT.reg = macAddress[4] || (macAddress[5] << 8);
}

void PHY_INTERRUPT_HANDLER(void) {
	sam_EICClearIRQ(pMacConfig->gpio_interrupt);
	
	if(pMacConfig->phyInterruptHandler != NULL) {
		pMacConfig->phyInterruptHandler();
	}

}

void PHY_POWEREVENT_HANDLER(void) {
	sam_EICClearIRQ(pMacConfig->gpio_powerEvent);
	
	if(pMacConfig->phyPowerEventHandler != NULL) {
		pMacConfig->phyPowerEventHandler();
	}
}