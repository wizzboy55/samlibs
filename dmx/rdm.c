/*
 * rdm.c
 *
 * Created: 2019-10-09 3:46:15 PM
 *  Author: charl
 */ 

#include "dmx.h"

RdmChecksum_t uRdmGetChecksum(DmxBuffer_t* buffer) {
	RdmChecksum_t checksum;
	
	checksum = buffer->rdm.message[buffer->rdm.messageLength] << 8 | buffer->rdm.message[buffer->rdm.messageLength + 1];
	
	return checksum;
}

RdmChecksum_t uRdmComputeChecksum(DmxBuffer_t* buffer) {
	RdmChecksum_t checksum = 0;
	
	for(uint8_t i = 0; i < buffer->rdm.messageLength; i++) {
		checksum += buffer->dmx[i];
	}
	
	return checksum;
}