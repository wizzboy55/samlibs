/*
 * rdm.h
 *
 * Created: 2019-10-09 3:28:43 PM
 *  Author: charl
 */ 


#ifndef RDM_H_
#define RDM_H_

#include "dmx.h"

RdmChecksum_t uRdmGetChecksum(DmxBuffer_t* buffer);
RdmChecksum_t uRdmComputeChecksum(DmxBuffer_t* buffer);

#endif /* RDM_H_ */