/*
 * samx5xmac_buffers.c
 *
 * Created: 2019-08-07 8:56:09 AM
 *  Author: Wizz
 */ 

#include "samx5xmac.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "NetworkInterface.h"
#include "NetworkBufferManagement.h"

#include "string.h"

static uint8_t ucEthRxBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ][ ETH_BUFFER_SIZE_ROUNDED_UP ];
static uint8_t ucEthTxBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ][ ETH_BUFFER_SIZE_ROUNDED_UP ];
extern SAMx5xMAC_ReceiveBuffer_t ethReceiveBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS];
extern SAMx5xMAC_TransmitBuffer_t ethTransmitBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS];

static NetworkBufferDescriptor_t xNetworkReceiveBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS];
static NetworkBufferDescriptor_t xNetworkTransmitBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS];

static SemaphoreHandle_t xNetworkTransmitBufferSemaphore = NULL;

/* A list of free (available) NetworkBufferDescriptor_t structures. */
static List_t xFreeTransmitBuffersList;

/* Some statistics about the use of buffers. */
static UBaseType_t uxMinimumFreeNetworkBuffers = 0u;

#if !defined( ipconfigBUFFER_ALLOC_LOCK )

	#define ipconfigBUFFER_ALLOC_INIT( ) do {} while (0)
	#define ipconfigBUFFER_ALLOC_LOCK_FROM_ISR()		\
		UBaseType_t uxSavedInterruptStatus = ( UBaseType_t ) portSET_INTERRUPT_MASK_FROM_ISR(); \
		{

	#define ipconfigBUFFER_ALLOC_UNLOCK_FROM_ISR()		\
			portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus ); \
		}

	#define ipconfigBUFFER_ALLOC_LOCK()					taskENTER_CRITICAL()
	#define ipconfigBUFFER_ALLOC_UNLOCK()				taskEXIT_CRITICAL()

#endif /* ipconfigBUFFER_ALLOC_LOCK */

void vSAMx5xInitializeBuffers(NetworkBufferDescriptor_t pxNetworkReceiveBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS], NetworkBufferDescriptor_t pxNetworkTransmitBuffers[ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS]) {
	
	memset(ethReceiveBuffers, 0, sizeof(ethReceiveBuffers));
	memset(ethTransmitBuffers, 0, sizeof(ethTransmitBuffers));

	for(BaseType_t i = 0; i < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; i++) {
		pxNetworkReceiveBuffers[i].pucEthernetBuffer = &(ucEthRxBuffers[i][ipBUFFER_PADDING]);
		pxNetworkReceiveBuffers[i].xDataLength = ETH_BUFFER_SIZE_ROUNDED_UP;
		// May not be required
		//*((uint32_t *) &ucEthRxBuffers[i][0]) = (uint32_t) &(pxNetworkReceiveBuffers[i]);

		ethReceiveBuffers[i].Word0.address = (uint32_t *)&(ucEthRxBuffers[i][0]);

		if(i == (ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS - 1)) {
			ethReceiveBuffers[i].Word0.flags.Wrap = 1;
		}
	}

	for(BaseType_t i = 0; i < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; i++) {
		pxNetworkTransmitBuffers[i].pucEthernetBuffer = &(ucEthTxBuffers[i][ipBUFFER_PADDING]);
		pxNetworkTransmitBuffers[i].xDataLength = ETH_BUFFER_SIZE_ROUNDED_UP;

		//*((uint32_t *) &ucEthTxBuffers[i][0]) = (uint32_t) &(pxNetworkTransmitBuffers[i]);

		ethTransmitBuffers[i].address = (uint32_t *)&(ucEthTxBuffers[i][0]);

		if(i == (ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS - 1)) {
			ethTransmitBuffers[i].Word1.bit.Wrap = 1;
		}
	}
}

BaseType_t xNetworkBuffersInitialise(void) {
	BaseType_t xReturn;

	if(xNetworkTransmitBufferSemaphore == NULL) {
		
		ipconfigBUFFER_ALLOC_INIT();

		xNetworkTransmitBufferSemaphore = xSemaphoreCreateCounting((UBaseType_t)ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS, (UBaseType_t) ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS);

		if(xNetworkTransmitBufferSemaphore != NULL) {
			
			vListInitialise(&xFreeTransmitBuffersList);

			vSAMx5xInitializeBuffers(xNetworkReceiveBuffers, xNetworkTransmitBuffers);

			for(BaseType_t x = 0; x < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; x++) {
				vListInitialiseItem(&(xNetworkTransmitBuffers[x].xBufferListItem));
				listSET_LIST_ITEM_OWNER(&(xNetworkTransmitBuffers[x].xBufferListItem), &xNetworkTransmitBuffers[x]);

				vListInsert(&xFreeTransmitBuffersList, &(xNetworkTransmitBuffers[x].xBufferListItem));
			}
		}

		uxMinimumFreeNetworkBuffers = ( UBaseType_t ) ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS;
	}

	if(xNetworkTransmitBufferSemaphore == NULL) {
		xReturn = pdFAIL;
	} else {
		xReturn = pdPASS;
	}

	return xReturn;
}

static UBaseType_t bIsValidNetworkDescriptor (const NetworkBufferDescriptor_t * pxDesc)
{
	( void ) pxDesc;
	return ( UBaseType_t ) pdTRUE;
}

NetworkBufferDescriptor_t* pxGetNetworkBufferWithDescriptor(size_t xRequestedSizeBytes, TickType_t xBlockTimeTicks) {
	NetworkBufferDescriptor_t *pxReturn = NULL;
	BaseType_t xInvalid = pdFALSE;
	UBaseType_t uxCount;

	( void ) xRequestedSizeBytes; // Unusued parameter

	if(xNetworkTransmitBufferSemaphore != NULL) {
		if(xSemaphoreTake(xNetworkTransmitBufferSemaphore, xBlockTimeTicks) == pdPASS) {
			
			ipconfigBUFFER_ALLOC_LOCK();
			{
				pxReturn = (NetworkBufferDescriptor_t*) listGET_OWNER_OF_HEAD_ENTRY(&xFreeTransmitBuffersList);

				if((bIsValidNetworkDescriptor(pxReturn) != pdFALSE_UNSIGNED) && listIS_CONTAINED_WITHIN(&xFreeTransmitBuffersList, &(pxReturn->xBufferListItem))) {
					uxListRemove(&(pxReturn->xBufferListItem));
				} else {
					xInvalid = pdTRUE;
				}
			}
			ipconfigBUFFER_ALLOC_UNLOCK();

			if(xInvalid == pdTRUE) {
				FreeRTOS_debug_printf( ("pxGetNetworkBufferWithDescriptor: INVALID BUFFER: &p (valid &lu)\n", pxReturn, bIsValidNetworkDescriptor(pxReturn)));
				pxReturn = NULL;
			} else {
				uxCount = listCURRENT_LIST_LENGTH(&xFreeTransmitBuffersList);

				if(uxMinimumFreeNetworkBuffers > uxCount) {
					uxMinimumFreeNetworkBuffers = uxCount;
				}

				// Not so sure about this...
				// pxReturn->xDataLength = xRequestedSizeBytes;

				#if( ipconfigUSE_LINKED_RX_MESSAGES != 0 )
				{
					pxReturn->pxNextBuffer = NULL;
				}
				#endif

				if( xTCPWindowLoggingLevel > 3 ) {
					FreeRTOS_debug_printf( ( "BUF_GET[%ld]: %p (%p)\n",	bIsValidNetworkDescriptor( pxReturn ), pxReturn, pxReturn->pucEthernetBuffer ) );
				}
			}
			iptraceNETWORK_BUFFER_OBTAINED( pxReturn );
		}
		else {
			iptraceFAILED_TO_OBTAIN_NETWORK_BUFFER();
		}
	}

	return pxReturn;
}

NetworkBufferDescriptor_t* pxNetworkBufferGetFromISR(size_t xRequestedSizeBytes) {
	return NULL;
}

BaseType_t vNetworkBufferReleaseFromISR(NetworkBufferDescriptor_t* const pxNetworkBuffer) {
	return pdFALSE;
}

void vReleaseNetworkBufferAndDescriptor(NetworkBufferDescriptor_t * const pxNetworkBuffer) {
	BaseType_t xListItemAlreadyInFreeList;

	if(bIsValidNetworkDescriptor(pxNetworkBuffer) == pdFALSE_UNSIGNED) {
		FreeRTOS_debug_printf( ( "vReleaseNetworkBufferAndDescriptor: Invalid buffer %p\n", pxNetworkBuffer ) );
		return;
	}

	ipconfigBUFFER_ALLOC_LOCK();
	{
		{
			xListItemAlreadyInFreeList = listIS_CONTAINED_WITHIN(&xFreeTransmitBuffersList, &(pxNetworkBuffer->xBufferListItem));

			if(xListItemAlreadyInFreeList == pdFALSE) {
				vListInsertEnd(&xFreeTransmitBuffersList, &(pxNetworkBuffer->xBufferListItem));
			}
		}
	}
	ipconfigBUFFER_ALLOC_UNLOCK();

	if(xListItemAlreadyInFreeList) {
		FreeRTOS_debug_printf( ( "vReleaseNetworkBufferAndDescriptor: %p ALREADY RELEASED (now %lu)\n", pxNetworkBuffer, uxGetNumberOfFreeNetworkBuffers( ) ) );
	}
	if(xListItemAlreadyInFreeList == pdFALSE) {
		xSemaphoreGive(xNetworkTransmitBufferSemaphore);
		prvShowWarnings();
		if( xTCPWindowLoggingLevel > 3 )
			FreeRTOS_debug_printf( ( "BUF_PUT[%ld]: %p (%p) (now %lu)\n", bIsValidNetworkDescriptor( pxNetworkBuffer ), pxNetworkBuffer, pxNetworkBuffer->pucEthernetBuffer, uxGetNumberOfFreeNetworkBuffers( ) ) );
	}
	iptraceNETWORK_BUFFER_RELEASED(pxNetworkBuffer);
}