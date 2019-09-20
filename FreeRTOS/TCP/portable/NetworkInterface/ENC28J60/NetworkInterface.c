/*
 * NetworkInterface.c
 *
 * Created: 2019-01-03 2:18:11 PM
 *  Author: Charles Gervais-Dumont
 */ 

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "hal_gpio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkBufferManagement.h"
#include "NetworkInterface.h"

#include "enc28j60.h"
#include "enc28j60_registers.h"

#include "utilities.h"

#include "debug_interface.h"

// OPTIONS
#define FULLDUPLEX		1
#define MAX_FRAMELEN	1536

#define ETH_TICKS_TO_WAIT (1000 / portTICK_PERIOD_MS)

static SemaphoreHandle_t semph_spi_free;
static TaskHandle_t nicTask;

struct {
	uint8_t linkUp;
	uint16_t nextPckPtr;
	uint8_t revId;
} macState;

static uint16_t erxrdpt_workaround(uint16_t next_packet_ptr, uint16_t start, uint16_t end) {
	return ((next_packet_ptr - 1 < start) || (next_packet_ptr - 1 > end)) ? end : (next_packet_ptr - 1);
}

static void ENC_rxFifoInit(void) {
	ENC_WriteCtrl16(J60_ERXSTL, RXSTART_INIT); // errata, must be zero
	ENC_WriteCtrl16(J60_ERXRDPTL, erxrdpt_workaround(RXSTART_INIT, RXSTART_INIT, RXSTOP_INIT));
	ENC_WriteCtrl16(J60_ERXNDL, RXSTOP_INIT);
	ENC_WriteCtrl16(J60_ERXWRPTL, RXSTART_INIT);
	macState.nextPckPtr = RXSTART_INIT;
}

static void ENC_txFifoInit(void) {
	ENC_WriteCtrl16(J60_ETXSTL, TXSTART_INIT);
	ENC_WriteCtrl16(J60_ETXNDL, TXSTOP_INIT);
	ENC_WriteCtrl16(J60_EWRPTL, TXSTART_INIT);
}

/**
* To get the MAC address
* @param mac
*/
void ETH_GetMAC(uint8_t * macAddr)
{
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR1);
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR2);
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR3);
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR4);
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR5);
	*macAddr++ = ENC_ReadCtrl8_MacMii(J60_MAADR6);
}

/**
 * To set the MAC address
 * @param mac
 */
void ETH_SetMAC(const uint8_t * macAddr)
{
	ENC_WriteOp(bfc_inst, J60_ECON1, ECON1_RXEN);

	ENC_WriteCtrl8(J60_MAADR1, *macAddr++);
	ENC_WriteCtrl8(J60_MAADR2, *macAddr++);
	ENC_WriteCtrl8(J60_MAADR3, *macAddr++);
	ENC_WriteCtrl8(J60_MAADR4, *macAddr++);
	ENC_WriteCtrl8(J60_MAADR5, *macAddr++);
	ENC_WriteCtrl8(J60_MAADR6, *macAddr++);

	ENC_WriteOp(bfs_inst, J60_ECON1, ECON1_RXEN);
}

 static void nicIntTask(void *p) {

	for(;;) {
		xTaskNotifyWait(0, 0, NULL, 500/portTICK_PERIOD_MS);	// Wait for interrupt or timeout

		if(xSemaphoreTake(semph_spi_free, ETH_TICKS_TO_WAIT) == pdFALSE) {// Wait for SPI to be free
			DEBUG_printf( ("ERR: LAN Semph Expired!\n") );
			continue;
		}

		uint8_t eir_state = ENC_ReadCtrl8(J60_EIR);

		// Check if link has changed
		if(eir_state & EIR_LINKIF) {
			ENC_ReadPhy(J60_PHIR);	// Clear LINKIF state by reading PHIR
			macState.linkUp = (ENC_ReadPhy(PHSTAT2) & PHSTAT2_LSTAT) == PHSTAT2_LSTAT;
		}

		while(ENC_ReadCtrl8(J60_EPKTCNT)) {	// While there is unread packets available
			// Copy Receive Status Vector
			enc28_recv_status_vector_t recv_vector;
			ENC_WriteCtrl16(J60_ERDPTL, macState.nextPckPtr);
			ENC_ReadBuffer((uint8_t*)&recv_vector, sizeof(recv_vector));

			macState.nextPckPtr = (recv_vector.nextPackterPointerH << 8) | recv_vector.nextPacketPointerL;

			IPStackEvent_t xRxEvent;
			NetworkBufferDescriptor_t *pxBufferDescriptor;
			size_t xBytesReceived = (recv_vector.status15_8 << 8) + recv_vector.status7_0;

			if(xBytesReceived > 0) {
				pxBufferDescriptor = pxGetNetworkBufferWithDescriptor(xBytesReceived, 0);

				if(pxBufferDescriptor != NULL) {
					// Copy data from ENC28J60 to allocated buffer
					ENC_ReadBuffer(pxBufferDescriptor->pucEthernetBuffer, xBytesReceived);
					pxBufferDescriptor->xDataLength = xBytesReceived;

					if(eConsiderFrameForProcessing(pxBufferDescriptor->pucEthernetBuffer) == eProcessBuffer) {
						xRxEvent.eEventType = eNetworkRxEvent;
						xRxEvent.pvData = (void*) pxBufferDescriptor;

						if(xSendEventStructToIPTask(&xRxEvent, 0) == pdFALSE) {
							// Could not send buffer to stack, needs to be released
							vReleaseNetworkBufferAndDescriptor(pxBufferDescriptor);
							iptraceETHERNET_RX_EVENT_LOST();
						} else {
							iptraceNETWORK_INTERFACE_RECEIVE();
						}
					} else {
						// Ethernet frame is dropped, must release buffer
						vReleaseNetworkBufferAndDescriptor(pxBufferDescriptor);
					}
				} else {
					iptraceETHERNET_RX_EVENT_LOST(); // Buffer not available
				}
			}
			ENC_WriteCtrl16(J60_ERXRDPTL, macState.nextPckPtr);
			ENC_WriteOp(bfs_inst, J60_ECON2, ECON2_PKTDEC);
		}

		if(eir_state & EIR_RXERIF) {
			ENC_WriteOp(bfc_inst, J60_EIR, EIR_RXERIF);
		}

		xSemaphoreGive(semph_spi_free);	// Done with SPI interface
	}
 }

void ext_irq_nic(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xTaskNotifyFromISR(nicTask, 0, eNoAction, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

 BaseType_t xNetworkInterfaceInitialise(void) {
	ENC_IfInit();

	ENC_Write8(src_inst);	// Send reset instruction
	vTaskDelay(50 / portTICK_PERIOD_MS); // As per Errata #2

	// Check Revision ID
	uint8_t rev = ENC_ReadCtrl8(J60_EREVID);
	macState.revId = rev;
	if (rev == 0 || rev == 0xFF) {	// SPI disconnected
		return pdFAIL;
	}

	if(!semph_spi_free) {
		semph_spi_free = xSemaphoreCreateBinary();
	}
	
	// Create Receive Task
	xTaskCreate(nicIntTask, "NicInt", 120, NULL, configMAX_PRIORITIES - 1, &nicTask);

	// Configuration
	ENC_WriteOp(bfs_inst, J60_ECON2, ECON2_AUTOINC | ECON2_VRPS);

	ENC_rxFifoInit();
	ENC_txFifoInit();

	const uint8_t fullDuplex = FULLDUPLEX;

	ENC_WriteCtrl8(J60_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_MPEN | ERXFCON_BCEN); // enable broadcast
	ENC_WriteCtrl16(J60_EPMM0, 0x303f);
	ENC_WriteCtrl16(J60_EPMCSL, 0xf7f9);

	// MAC INIT
	ENC_WriteCtrl8(J60_MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
	ENC_WriteCtrl8(J60_MACON2, 0x00);
	ENC_WriteCtrl8(J60_MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | (fullDuplex ? MACON3_FULDPX : 0));
	ENC_WriteCtrl8(J60_MACON4, MACON4_DEFER);
	ENC_WriteCtrl16(J60_MAMXFLL, MAX_FRAMELEN);
	ENC_WriteCtrl8(J60_MABBIPG, fullDuplex ? 0x15 : 0x12);
	ENC_WriteCtrl16(J60_MAIPGL, fullDuplex ? 0x0012 : 0x0C12);
	// MACLCON1 unchanged
	// MACLCON2 unchanged

	// PHY INIT
	ENC_WritePhy(PHCON1, fullDuplex ? PHCON1_PDPXMD : 0);
	ENC_WritePhy(PHCON2, fullDuplex ? 0 : PHCON2_HDLDIS);
	// PHLCON unchanged (LED behavior)
	ENC_WritePhy(PHIE, PHIE_PGEIE | PHIE_PLNKIE);

	const uint8_t * _macAddr;
	_macAddr = FreeRTOS_GetMACAddress();
	ETH_SetMAC(_macAddr);	// RXEN is set in this function

	xSemaphoreGive(semph_spi_free);

	ENC_WriteCtrl8(J60_EIE, EIE_INTIE | EIE_PKTIE | EIE_LINKIE);
	ENC_RegisterIrq(ext_irq_nic);

	return pdPASS;
}

 BaseType_t xNetworkInterfaceOutput(xNetworkBufferDescriptor_t * const pxDescriptor, BaseType_t xReleaseAfterSend) {

	if(xSemaphoreTake(semph_spi_free, ETH_TICKS_TO_WAIT) == pdFALSE) {
		DEBUG_printf( ("ERR: Send Semph Timeout!\n") );
		return pdFALSE;
	}

	delay_ms(1);

	if (macState.linkUp) {
		// Check if already transmitting
		if(ENC_ReadCtrl8(J60_ECON1) & ECON1_TXRTS) {
			// Is already transmitting
			if (xReleaseAfterSend) {
				vReleaseNetworkBufferAndDescriptor(pxDescriptor);
			}
			return pdFAIL;
		}
		ENC_txFifoInit();
		// Write frame control byte
		uint8_t txctrl = 0x00;
		// Write frame data
		ENC_WriteBuffer(&txctrl, 1);
		ENC_WriteBuffer(pxDescriptor->pucEthernetBuffer, pxDescriptor->xDataLength);
		uint16_t _etxst = ENC_ReadCtrl16(J60_ETXSTL);
		uint16_t _etxnd = _etxst + pxDescriptor->xDataLength + 1;
		ENC_WriteCtrl16(J60_ETXNDL, _etxnd);
		// Clear Interrupt flags
		ENC_WriteOp(bfc_inst, J60_EIR, EIR_TXIF);
		// Transmit
		ENC_WriteOp(bfs_inst, J60_ECON1, ECON1_TXRTS);
	}

	if (xReleaseAfterSend) {
		vReleaseNetworkBufferAndDescriptor(pxDescriptor);
	}
	/* Call the standard trace macro to log the send event. */
	iptraceNETWORK_INTERFACE_TRANSMIT();

	xSemaphoreGive(semph_spi_free);

	return pdTRUE;
 }

 #define BUFFER_SIZE (ipTOTAL_ETHERNET_FRAME_SIZE + ipBUFFER_PADDING)
 #define BUFFER_SIZE_ROUNDED_UP ((BUFFER_SIZE + 7) & ~0x07UL)
 static uint8_t ucBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS][BUFFER_SIZE_ROUNDED_UP];

void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] ) {
	BaseType_t x;

    for( x = 0; x < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; x++ )
    {
        /* pucEthernetBuffer is set to point ipBUFFER_PADDING bytes in from the
        beginning of the allocated buffer. */
        pxNetworkBuffers[ x ].pucEthernetBuffer = &( ucBuffers[ x ][ ipBUFFER_PADDING ] );

        /* The following line is also required, but will not be required in
        future versions. */
        *( ( uint32_t * ) &ucBuffers[ x ][ 0 ] ) = ( uint32_t ) &( pxNetworkBuffers[ x ] );
    }
}