/*
 * IEEEClause22Registers.h
 *
 * Created: 2019-07-30 3:28:53 PM
 *  Author: Wizz
 */ 


#ifndef IEEECLAUSE22REGISTERS_H_
#define IEEECLAUSE22REGISTERS_H_

// IEEE Defined Registers
#define Clause22_BasicControl						0x00
#define Clause22_BasicStatus						0x01
#define Clause22_PHYIdentifier1						0x02
#define Clause22_PHYIdentifier2						0x03
#define Clause22_AutoNegotiationAdvertisement		0x04
#define Clause22_AutoNegotiationLinkPartner			0x05
#define Clause22_AutoNegotiationExpansion			0x06
#define Clause22_AutoNegotiationNextPage			0x07
#define Clause22_AutoNegotiationLinkPartnerNextPage	0x08
#define Clause22_MMDAccessControl					0x0D
#define Clause22_MMDAccessRegisterData				0x0E

typedef union {
	struct {
		uint16_t :7;
		uint16_t CollisionTest:1;
		uint16_t DuplexMode:1;
		uint16_t RestartAutoNegotiation:1;
		uint16_t Isolate:1;
		uint16_t PowerDown:1;
		uint16_t AutoNegotiationEnable:1;
		uint16_t SpeedSelect:1;
		uint16_t Loopback:1;
		uint16_t Reset:1;
	} bit;
	uint16_t reg;
} Clause22_BasicControl_t;

typedef union {
	struct {
		uint16_t ExtendedCapability:1;
		uint16_t JabberDetect:1;
		uint16_t LinkStatus:1;
		uint16_t AutoNegotiationAbility:1;
		uint16_t RemoteFault:1;
		uint16_t AutoNegotiationComplete:1;
		uint16_t NoPreamble:1;
		uint16_t :4;
		uint16_t b10BaseTHalfDuplex:1;
		uint16_t b10BaseTFullDuplex:1;
		uint16_t b100BaseTXHalfDuplex:1;
		uint16_t b100BaseTXFullDuplex:1;
		uint16_t b100BaseT4:1;
	} bit;
	uint16_t reg;
} Clause22_BasicStatus_t;

typedef uint16_t Clause22_PHYIdentifier1_t;

typedef union {
	struct {
		uint16_t RevisionNumber:4;
		uint16_t ModelNumber:6;
		uint16_t PHYIDNumber:6;
	} bit;
	uint16_t reg;
} Clause22_PHYIdentifier2_t;

typedef union {
	struct {
		uint16_t SelectorField:5;
		uint16_t b10BaseTHalfDuplex:1;
		uint16_t b10BaseTFullDuplex:1;
		uint16_t b100BaseTXHalfDuplex:1;
		uint16_t b100BaseTXFullDuplex:1;
		uint16_t b100BaseT4:1;
		uint16_t Pause:2;
		uint16_t :1;
		uint16_t RemoteFault:1;
		uint16_t :1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} Clause22_AutoNegotiationAdvertisement_t;

typedef union {
	struct {
		uint16_t SelectorField:5;
		uint16_t b10BaseTHalfDuplex:1;
		uint16_t b10BaseTFullDuplex:1;
		uint16_t b100BaseTXHalfDuplex:1;
		uint16_t b100BaseTXFullDuplex:1;
		uint16_t b100BaseT4:1;
		uint16_t Pause:2;
		uint16_t :1;
		uint16_t RemoteFault:1;
		uint16_t Acknowledge:1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} Clause22_AutoNegotiationLinkPartner_t;

typedef union {
	struct {
		uint16_t LinkPartnerAutoNegotiationAble:1;
		uint16_t PageReceived:1;
		uint16_t NextPageAble:1;
		uint16_t LinkPartnerNextPageAble:1;
		uint16_t ParallelDetectionFault:1;
		uint16_t :11;
	} bit;
	uint16_t reg;
} Clause22_AutoNegotiationExpansion_t;

typedef union {
	struct {
		uint16_t MessageField:11;
		uint16_t Toggle:1;
		uint16_t Acknowledge2:1;
		uint16_t MessagePage:1;
		uint16_t :1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} Clause22_AutoNegotiationNextPage_t;

typedef union {
	struct {
		uint16_t MessageField:11;
		uint16_t Toggle:1;
		uint16_t Acknowledge2:1;
		uint16_t MessagePage:1;
		uint16_t Acknowledge:1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} Clause22_AutoNegotiationLinkPartnerNextPage_t;

typedef union {
	struct {
		uint16_t DeviceAddress:5;
		uint16_t :9;
		uint16_t OperationMode:2;
	} bit;
	uint16_t reg;
} Clause22_MMDAccessControl_t;

typedef uint16_t Clause22_MMDAccessRegisterData_t;

#define Clause22_SelectorField_IEEE802_3 0b00001
#define Clause22_SelectorField_IEEE802_9 0b00010
#define Clause22_SelectorField_IEEE802_5 0b00011
#define Clause22_SelectorField_IEEE1394  0b00100
#define Clause22_SelectorField_INCITS	 0b00101

#endif /* IEEECLAUSE22REGISTERS_H_ */