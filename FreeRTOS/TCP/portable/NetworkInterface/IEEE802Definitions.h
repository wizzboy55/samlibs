/*
 * IEEE802Definitions.h
 *
 * Created: 2019-08-20 10:02:37 AM
 *  Author: Wizz
 */ 


#ifndef IEEE802DEFINITIONS_H_
#define IEEE802DEFINITIONS_H_

typedef enum IEEELinkCapability_e {None, Half10BaseT, Full10BaseT, Half100BaseTX, Half100BaseT4, Half100BaseT2, Full100BaseTX, Full100BaseT2, Half1000BaseT, Full1000BaseT, Full2_5GBaseT, Full5GBaseT, Full10GBaseT, Full25GBaseT, Full40GBaseT } eIEEELinkCapability_t;

typedef union {
	struct {
		uint32_t lcHalf10BaseT:1;
		uint32_t lcFull10BaseT:1;
		uint32_t lcHalf100BaseTX:1;
		uint32_t lcHalf100BaseT4:1;
		uint32_t lcHalf100BaseT2:1;
		uint32_t lcFull100BaseTX:1;
		uint32_t lcFull100BaseT2:1;
		uint32_t lcHalf1000BaseT:1;
		uint32_t lcFull1000BaseT:1;
		uint32_t lcFull2_5GBaseT:1;
		uint32_t lcFull5GBaseT:1;
		uint32_t lcFull10GBaseT:1;
		uint32_t lcFull25GBaseT:1;
		uint32_t lcFull40GBaseT:1;
		uint32_t :18;
	} bit;
	uint32_t reg;
} IEEELinkCapabilities_t;

#endif /* IEEE802DEFINITIONS_H_ */