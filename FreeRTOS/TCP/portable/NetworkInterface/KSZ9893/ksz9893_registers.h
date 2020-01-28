/*
 * ksz9893_registers.h
 *
 * Created: 2020-01-24 9:37:44 AM
 *  Author: charl
 */ 


#ifndef KSZ9893_REGISTERS_H_
#define KSZ9893_REGISTERS_H_

#include <stdint.h>

#define KSZ9893_GlobalChipID0						0x0000
#define KSZ9893_GlobalChipID1						0x0001
#define KSZ9893_GlobalChipID2						0x0002
#define KSZ9893_GlobalChipID3						0x0003
#define KSZ9893_PMEPinControl						0x0006
#define KSZ9893_GlobalChipID4						0x000F
#define KSZ9893_GlobalInterruptStatus				0x0010
#define KSZ9893_GlobalInterruptMask					0x0014
#define KSZ9893_GlobalPortInterruptStatus			0x0018
#define KSZ9893_GlobalPortInterruptMask				0x001C
#define KSZ9893_SerialIOControl						0x0100
#define KSZ9893_InBandManagementControl				0x0104
#define KSZ9893_IODriveStrength						0x010D
#define KSZ9893_InBandManagementOperationStatus1	0x0110
#define KSZ9893_LEDOverride							0x0120
#define KSZ9893_LEDOutput							0x0124
#define KSZ9893_PowerDownControl0					0x0201
#define KSZ9893_LEDStrapIn							0x0210
#define KSZ9893_SwitchOperation						0x0300
#define KSZ9893_SwitchMACAddress0					0x0302
#define KSZ9893_SwitchMACAddress1					0x0303
#define KSZ9893_SwitchMACAddress2					0x0304
#define KSZ9893_SwitchMACAddress3					0x0305
#define KSZ9893_SwitchMACAddress4					0x0306
#define KSZ9893_SwitchMACAddress5					0x0307
#define KSZ9893_SwitchMaximumTransmitUnit			0x0308
#define KSZ9893_SwitchISPTPID						0x030A
#define KSZ9893_SwitchLookupEngineControl0			0x0310
#define KSZ9893_SwitchLookupEngineControl1			0x0311
#define KSZ9893_SwitchLookupEngineControl2			0x0312
#define KSZ9893_SwitchLookupEngineControl3			0x0313
#define KSZ9893_AddressLookupTableInterrupt			0x0314	// TODO: Add register descriptions below
#define KSZ9893_AddressLookupTableMask				0x0315
#define KSZ9893_AddressLookupTableEntryIndex0		0x0316
#define KSZ9893_AddressLookupTableEntryIndex1		0x0318
#define KSZ9893_AddressLookupTableEntryIndex2		0x031A
#define KSZ9893_UnknownUnicastControl				0x0320
#define KSZ9893_UnknownMulticastControl				0x0324
#define KSZ9893_UnknownVLANIDControl				0x0328
#define KSZ9893_SwitchMACControl0					0x0330
#define KSZ9893_SwitchMACControl1					0x0331
#define KSZ9893_SwitchMACControl2					0x0332
#define KSZ9893_SwitchMACControl3					0x0333
#define KSZ9893_SwitchMACControl4					0x0334
#define KSZ9893_SwitchMACControl5					0x0335
#define KSZ9893_SwitchMIBControl					0x0336
#define KSZ9893_8021pPriorityMapping0				0x0338
#define KSZ9893_8021pPriorityMapping1				0x0339
#define KSZ9893_8021pPriorityMapping2				0x033A
#define KSZ9893_8021pPriorityMapping3				0x033B
#define KSZ9893_IPDiffServPriorityEnable			0x033E
#define KSZ9893_IPDiffServPriorityMapping0			0x0340
#define KSZ9893_IPDiffServPriorityMapping1			0x0341
#define KSZ9893_IPDiffServPriorityMapping2			0x0342
#define KSZ9893_IPDiffServPriorityMapping3			0x0343
#define KSZ9893_IPDiffServPriorityMapping4			0x0344
#define KSZ9893_IPDiffServPriorityMapping5			0x0345
#define KSZ9893_IPDiffServPriorityMapping6			0x0346
#define KSZ9893_IPDiffServPriorityMapping7			0x0347
#define KSZ9893_IPDiffServPriorityMapping8			0x0348
#define KSZ9893_IPDiffServPriorityMapping9			0x0349
#define KSZ9893_IPDiffServPriorityMapping10			0x034A
#define KSZ9893_IPDiffServPriorityMapping11			0x034B
#define KSZ9893_IPDiffServPriorityMapping12			0x034C
#define KSZ9893_IPDiffServPriorityMapping13			0x034D
#define KSZ9893_IPDiffServPriorityMapping14			0x034E
#define KSZ9893_IPDiffServPriorityMapping15			0x034F
#define KSZ9893_IPDiffServPriorityMapping16			0x0350
#define KSZ9893_IPDiffServPriorityMapping17			0x0351
#define KSZ9893_IPDiffServPriorityMapping18			0x0352
#define KSZ9893_IPDiffServPriorityMapping19			0x0353
#define KSZ9893_IPDiffServPriorityMapping20			0x0354
#define KSZ9893_IPDiffServPriorityMapping21			0x0355
#define KSZ9893_IPDiffServPriorityMapping22			0x0356
#define KSZ9893_IPDiffServPriorityMapping23			0x0357
#define KSZ9893_IPDiffServPriorityMapping24			0x0358
#define KSZ9893_IPDiffServPriorityMapping25			0x0359
#define KSZ9893_IPDiffServPriorityMapping26			0x035A
#define KSZ9893_IPDiffServPriorityMapping27			0x035B
#define KSZ9893_IPDiffServPriorityMapping28			0x035C
#define KSZ9893_IPDiffServPriorityMapping29			0x035D
#define KSZ9893_IPDiffServPriorityMapping30			0x035E
#define KSZ9893_IPDiffServPriorityMapping31			0x035F
#define KSZ9893_GlobalPortMirroringAndSnoppingControl	0x0370
#define KSZ9893_WREDDiffServColorMapping			0x0378
#define KSZ9893_QueueManagementControl0				0x0390
#define KSZ9893_VLANTableEntry0						0x0400
#define KSZ9893_VLANTableEntry1						0x0404
#define KSZ9893_VLANTableEntry2						0x0408
#define KSZ9893_VLANTableIndex						0x040C
#define KSZ9893_VLANTableAccessControl				0x040E
#define KSZ9893_ALUTableIndex0						0x0410
#define KSZ9893_ALUTableIndex1						0x0414
#define KSZ9893_ALUTableAccessControl				0x0418
#define KSZ9893_StaticAddressAndReservedMulticastTableControl	0x041C
#define KSZ9893_ALUStaticAddressTableEntry1			0x0420
#define KSZ9893_ALUStaticAddressReservedMulticastTableEntry2	0x0424
#define KSZ9893_ALUStaticAddressTableEntry3			0x0428
#define KSZ9893_ALUStaticAddressTableEntry4			0x042C					// TODO: Up to here
#define KSZ9893_Port1Offset							0x1000
#define KSZ9893_Port2Offset							0x2000
#define KSZ9893_Port3Offset							0x3000
#define KSZ9893_PortNDefaultTag0					0x000
#define KSZ9893_PortNDefaultTag1					0x001
#define KSZ9893_PortNPMEWoLEvent					0x013
#define KSZ9893_PortNPMEWoLEnable					0x017
#define KSZ9893_PortNInterruptStatus				0x01B
#define KSZ9893_PortNInterruptMask					0x01F
#define KSZ9893_PortNOperationControl0				0x020
#define KSZ9893_PortNStatus							0x030
#define KSZ9893_PortNPHYBasicControl				0x100
#define KSZ9893_PortNPHYBasicStatus					0x102
#define KSZ9893_PortNPHYIDHigh						0x104
#define KSZ9893_PortNPHYIDLow						0x106
#define KSZ9893_PortNAutoNegociationAdvertisement	0x108
#define KSZ9893_PortNAutoNegociationLinkPartnerAbility	0x10A
#define KSZ9893_PortNPHYAutoNegociationExpansionStatus	0x10C
#define KSZ9893_PortNPHYAutoNegociationNextPage		0x10E
#define KSZ9893_PortNPHYAutoNegociationLinkPartnerNextPageAbility	0x110
#define KSZ9893_PortNPHY1000BASETControl			0x112
#define KSZ9893_PortNPHY1000BASETStatus				0x114
#define KSZ9893_PortNPHYMMDSetupRegister			0x11A
#define KSZ9893_PortNPHYMMDData						0x11C
#define KSZ9893_PortNPHYExtendedStatus				0x11E
#define KSZ9893_PortNPHYRemoteLoopback				0x122
#define KSZ9893_PortNPHYLinkMD						0x124
#define KSZ9893_PortNDigitalPMAPCSStatus			0x126
#define KSZ9893_PortNRXERCount						0x12A
#define KSZ9893_PortNInterruptControlStatus			0x136
#define KSZ9893_PortNPHYAutoMDIMDIX					0x138
#define KSZ9893_PortNPHYControl						0x13E
#define KSZ9893_Port3XMIIPortControl0				0x3300
#define KSZ9893_Port3XMIIPortControl1				0x3301
#define KSZ9893_Port3XMIIPortControl3				0x3303
#define KSZ9893_PortNMACControl0					0x400
#define KSZ9893_PortNMACControl1					0x401
#define KSZ9893_PortNIngressRateLimitControl		0x403
#define KSZ9893_PortNPriority0IngressLimitControl	0x410
#define KSZ9893_PortNPriority1IngressLimitControl	0x411
#define KSZ9893_PortNPriority2IngressLimitControl	0x412
#define KSZ9893_PortNPriority3IngressLimitControl	0x413
#define KSZ9893_PortNPriority4IngressLimitControl	0x414
#define KSZ9893_PortNPriority5IngressLimitControl	0x415
#define KSZ9893_PortNPriority6IngressLimitControl	0x416
#define KSZ9893_PortNPriority7IngressLimitControl	0x417
#define KSZ9893_PortNQueue0EgressLimitControl		0x420
#define KSZ9893_PortNQueue1EgressLimitControl		0x421
#define KSZ9893_PortNQueue2EgressLimitControl		0x422
#define KSZ9893_PortNQueue3EgressLimitControl		0x423
#define KSZ9893_PortNMIBControlAndStatus			0x500
#define KSZ9893_PortNMIBData						0x504
#define KSZ9893_PortNACLAccess0						0x600
#define KSZ9893_PortNACLAccess1						0x601
#define KSZ9893_PortNACLAccess2						0x602
#define KSZ9893_PortNACLAccess3						0x603
#define KSZ9893_PortNACLAccess4						0x604
#define KSZ9893_PortNACLAccess5						0x605
#define KSZ9893_PortNACLAccess6						0x606
#define KSZ9893_PortNACLAccess7						0x607
#define KSZ9893_PortNACLAccess8						0x608
#define KSZ9893_PortNACLAccess9						0x609
#define KSZ9893_PortNACLAccessA						0x60A
#define KSZ9893_PortNACLAccessB						0x60B
#define KSZ9893_PortNACLAccessC						0x60C
#define KSZ9893_PortNACLAccessD						0x60D
#define KSZ9893_PortNACLAccessE						0x60E
#define KSZ9893_PortNACLAccessF						0x60F
#define KSZ9893_PortNACLByteEnableMSB				0x610
#define KSZ9893_PortNACLByteEnableLSB				0x611
#define KSZ9893_PortNACLAccessControl0				0x612
#define KSZ9893_PortNMirroringControl				0x800
#define KSZ9893_PortNPriorityControl				0x801
#define KSZ9893_PortNIngressMACControl				0x802
#define KSZ9893_PortNAuthenticationControl			0x803
#define KSZ9893_PortNPointer						0x804
#define KSZ9893_PortNPriorityToQueueMapping			0x808
#define KSZ9893_PortNPoliceControl					0x80C
#define KSZ9893_PortNTransmitQueueIndex				0x900
#define KSZ9893_PortNTransmitQueuePVID				0x904
#define KSZ9893_PortNTransmitQueueControl0			0x914
#define KSZ9893_PortNTransmitQueueControl1			0x915
#define KSZ9893_PortNControl0						0xA00
#define KSZ9893_PortNControl1						0xA04
#define KSZ9893_PortNControl2						0xB00
#define KSZ9893_PortNMSTPPointer					0xB01
#define KSZ9893_PortNMSTPState						0xB04

typedef union {
	struct {
		uint8_t GlobalSoftwareReset:1;
		uint8_t :3;
		uint8_t RevisionID:4;
	} bit;
	uint8_t reg;
} KSZ9893_GlobalChipID3_t;

typedef union {
	struct {
		uint8_t PMEPinOutputPolarity:1;
		uint8_t PMEPinOutputEnable:1;
		uint8_t :6;
	} bit;
	uint8_t reg;
} KSZ9893_PMEPinControl_t;

typedef union {
	struct {
		uint32_t :31;
		uint32_t LookupEngineInterruptStatus:1;
	} bit;
	uint32_t reg;
} KSZ9893_GlobalInterruptStatus_t;

typedef union {
	struct {
		uint32_t :31;
		uint32_t LookupEngineInterruptMask:1;
	} bit;
	uint32_t reg;
} KSZ9893_GlobalInterruptMask_t;

typedef union {
	struct {
		uint32_t Port1InterruptStatus:1;
		uint32_t Port2InterruptStatus:1;
		uint32_t Port3InterruptStatus:1;
		uint32_t :29;
	} bit;
	uint32_t reg;
} KSZ9893_GlobalPortInterruptStatus_t;

typedef union {
	struct {
		uint32_t Port1InterruptMask:1;
		uint32_t Port2InterruptMask:1;
		uint32_t Port3InterruptMask:1;
		uint32_t :29;
	} bit;
	uint32_t reg;
} KSZ9893_GlobalPortInterruptMask_t;

typedef union {
	struct {
		uint8_t SPIDataOutEdgeSelect:1;
		uint8_t AutomaticSPIDataOutEdgeSelect:1;
		uint8_t MIIMPreambleSuppression:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_SerialIOControl_t;

typedef union {
	struct {
		uint32_t TPIDValueForIBAFrameHeader:16;
		uint32_t PortUsedForIBACommunication:3;
		uint32_t :3;
		uint32_t PriorityQueueForIBAResponse:2;
		uint32_t :5;
		uint32_t IBAReset:1;
		uint32_t IBADestinationMACAddressMatchEnable:1;
		uint32_t IBAEnable:1;
	} bit;
	uint32_t reg;
} KSZ9893_InBandManagementControl_t;

typedef union {
	struct {
		uint8_t LowSpeedDriveStrength:3;
		uint8_t :1;
		uint8_t HighSpeedDriveStrength:3;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_IODriveStrength_t;

typedef union {
	struct {
		uint32_t IBAAccessCodeErrorLocation:7;
		uint32_t :3;
		uint32_t IBAOversizePacketError:1;
		uint32_t IBAAcessCommandError:1;
		uint32_t IBAAccessCodeError:1;
		uint32_t IBAAccessFormatError:1;
		uint32_t IBAMACAddressMismatchError:1;
		uint32_t :14;
		uint32_t IBAExecutionDone:1;
		uint32_t IBAResponsePacketTransmitDone:1;
		uint32_t GoodIBAPacketDetect:1;
	} bit;
	uint32_t reg;
} KSZ9893_InBandManagementOperationStatus1_t;

typedef union {
	struct {
		uint32_t OverrideLED:4;
		uint32_t :28;
	} bit;
	uint32_t reg;
} KSZ9893_LEDOverride_t;

typedef union {
	struct {
		uint32_t GPOOutputControl:4;
		uint32_t :28;
	} bit;
	uint32_t reg;
} KSZ9893_LEDOutput_t;

typedef union {
	struct {
		uint8_t :3;
		uint8_t PowerManagementMode:2;
		uint8_t PLLPowerDown:1;
		uint8_t :2;
	} bit;
	uint8_t reg;
} KSZ9893_PowerDownControl0_t;

typedef union {
	struct {
		uint32_t StrapInValuesOfSelectRXDandLEDPins:4;
		uint32_t :28;
	} bit;
	uint32_t reg;
} KSZ9893_LEDStrapIn_t;

typedef union {
	struct {
		uint8_t StartSwitch:1;
		uint8_t SoftHardwareReset:1;
		uint8_t :5;
		uint8_t DoubleTagEnable;
	} bit;
	uint8_t reg;
} KSZ9893_SwitchOperation_t;

typedef struct {
	uint8_t MACAddress5;
	uint8_t MACAddress4;
	uint8_t MACAddress3;
	uint8_t MACAddress2;
	uint8_t MACAddress1;
	uint8_t MACAddress0;
} KSZ9893_SwitchMACAddress_t;

typedef union {
	struct {
		uint16_t MaximumFrameLength:14;
		uint16_t :2;
	} bit;
	uint16_t reg;
} KSZ9893_SwitchMaximumTransmitUnit_t;

typedef union {
	struct {
		uint8_t HASH_OPTION:2;
		uint8_t ReservedMulticastLookupEnable:1;
		uint8_t AgeCount:3;
		uint8_t DropInvalidVID:1;
		uint8_t IEEE8021QVLANEnable:1;
	} bit;
	uint8_t reg;
} KSZ9893_SwitchLookupEngineControl0_t;

typedef union {
	struct {
		uint8_t LinkDownFlush:1;
		uint8_t FastAging:1;
		uint8_t AgingEnable:1;
		uint8_t MulticastSourceAddressFiltering:1;
		uint8_t FlushMSTPAddressEntries:1;
		uint8_t FlushAddressLookupTable:1;
		uint8_t SelfAddressFiltering_GlobalEnable:1;
		uint8_t UnicastLearningDisable:1;
	} bit;
	uint8_t reg;
} KSZ9893_SwitchLookupEngineControl1_t;

typedef union {
	struct {
		uint8_t MACAddressPriority:2;
		uint8_t FlushOption:2;
		uint8_t StaticEntryEgressVLANFiltering:1;
		uint8_t DynamicEntryEgressVLANFiltering:1;
		uint8_t DoubleTagMulticastTrap:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_SwitchLookupEngineControl2_t;

typedef union {
	struct {
		uint8_t AgePeriod;
	} bit;
	uint8_t reg;
} KSZ9893_SwitchLookupEngineControl3_t;

// TODO: Add registers

typedef union {
	struct {
		uint8_t VLANIdentifier11_8:4;
		uint8_t DropEligibleIndicate:1;
		uint8_t PriorityCodePoint:3;
	} bit;
	uint8_t reg;
} KSZ9893_PortNDefaultTag0_t;

typedef union {
	struct {
		uint8_t VLANIdentifier7_0;
	} bit;
	uint8_t reg;
} KSZ9893_PortNDefaultTag1_t;

typedef union {
	struct {
		uint8_t EnergyDetect:1;
		uint8_t LinkUpDetect:1;
		uint8_t MagicPacketDetect:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNPMEWoLEvent_t;

typedef union {
	struct {
		uint8_t EnergyDetectEnable:1;
		uint8_t LinkUpDetectEnable:1;
		uint8_t MagicPacketDetectEnable:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNPMEWoLEnable_t;

typedef union {
	struct {
		uint8_t ACLInterruptStatus:1;
		uint8_t PHYInterruptStatus:1;
		uint8_t :6;
	} bit;
	uint8_t reg;
} KSZ9893_PortNInterruptStatus_t;

typedef union {
	struct {
		uint8_t ACLInterruptEnable:1;
		uint8_t PHYInterruptEnable:1;
		uint8_t :6;
	} bit;
	uint8_t reg;
} KSZ9893_PortNInterruptMask_t;

typedef union {
	struct {
		uint8_t EgressQueueSplitEnable:2;
		uint8_t TailTagEnable:1;
		uint8_t :3;
		uint8_t RemoteMACLoopback:1;
		uint8_t LocalMACLoopback:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNOperationControl0_t;

typedef union {
	struct {
		uint8_t ReceiveFlowControlEnabledStatus:1;
		uint8_t TransmitFlowControlEnabledStatus:1;
		uint8_t PortDuplexStatus:1;
		uint8_t PortSpeedStatus:2;
		uint8_t :3;
	} bit;
	uint8_t reg;
} KSZ9893_PortNStatus_t;

typedef union {
	struct {
		uint16_t :6;
		uint16_t SpeedSelectMSB:1;
		uint16_t CollisionTest:1;
		uint16_t DuplexMode:1;
		uint16_t ReStartAutoNegociation:1;
		uint16_t Isolate:1;
		uint16_t PowerDown:1;
		uint16_t AutoNegociationEnable:1;
		uint16_t SpeedSelectLSB:1;
		uint16_t LocalLoopbackMode:1;
		uint16_t PHYSoftwareReset:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYBasicControl_t;

typedef union {
	struct {
		uint16_t ExtendedCapability:1;
		uint16_t JabberDetect:1;
		uint16_t LinkStatus:1;
		uint16_t AutoNegociationAbility:1;
		uint16_t RemoteFault:1;
		uint16_t AutoNegociationComplete:1;
		uint16_t MFPreambleSuppression:1;
		uint16_t :1;
		uint16_t ExtendedStatus:1;
		uint16_t :2;
		uint16_t _10MbsHalfDuplex:1;
		uint16_t _10MbsFullDuplex:1;
		uint16_t _100BASETXHalfDuplex:1;
		uint16_t _100BASETXFullDuplex:1;
		uint16_t _100BASTET4:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYBasicStatus_t;

typedef union {
	struct {
		uint16_t PHYIdentifierHighWord;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYIDHigh_t;

typedef union {
	struct {
		uint16_t PHYIdentifierLowWord;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYIDLow_t;

typedef union {
	struct {
		uint16_t SelectorField:5;
		uint16_t _10BASETHalfDuplex:1;
		uint16_t _10BASETFullDuplex:1;
		uint16_t _100BASETXHalfDuplex:1;
		uint16_t _100BASETXFullDuplex:1;
		uint16_t _100BASET4:1;
		uint16_t PauseCapability:2;
		uint16_t :1;
		uint16_t RemoteFault:1;
		uint16_t :1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNAutoNegociationAdvertisement_t;

typedef union {
	struct {
		uint16_t SelectorField:5;
		uint16_t _10BASETHalfDuplex:1;
		uint16_t _10BASETFullDuplex:1;
		uint16_t _100BASETXHalfDuplex:1;
		uint16_t _100BASETXFullDuplex:1;
		uint16_t _100BASET4:1;
		uint16_t PauseCapability:2;
		uint16_t :1;
		uint16_t RemoteFault:1;
		uint16_t Acknowledge:1;
		uint16_t NextPage:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNAutoNegociationLinkPartnerAbility_t;

typedef union {
	struct {
		uint16_t LinkPartnerAutoNegociationAble:1;
		uint16_t PageReceived:1;
		uint16_t NextPageAble:1;
		uint16_t LinkPartnerNextPageAble:1;
		uint16_t ParallelDetectionFault:1;
		uint16_t :11;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYAutoNegociationExpansionStatus_t;

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
} KSZ9893_PortNPHYAutoNegociationNextPage_t;

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
} KSZ9893_PortNPHYAutoNegociationLinkPartnerNextPageAbility_t;


typedef union {
	struct {
		uint16_t :8;
		uint16_t _1000BASETHalfDuplex:1;
		uint16_t _1000BASETFullDuplex:1;
		uint16_t PortType:1;
		uint16_t MasterSlaveManualConfigurationValue:1;
		uint16_t MasterSlaveManualConfigurationEnable:1;
		uint16_t TransmitterTestModeOperations:3;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHY1000BASETControl_t;

typedef union {
	struct {
		uint16_t IdleErrorCount:8;
		uint16_t :2;
		uint16_t LinkPartner1000BASETHalfDuplexCapability:1;
		uint16_t LinkPartner1000BASETFullDuplexCapability:1;
		uint16_t RemoteReceiverStatus:1;
		uint16_t LocalReceiverStatus:1;
		uint16_t MasterSlaveConfigurationResolution:1;
		uint16_t MasterSlaveConfigurationFault:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHY1000BASETStatus_t;

typedef union {
	struct {
		uint16_t MMDDeviceAddress:5;
		uint16_t :9;
		uint16_t MMDOperationMode:2;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYMMDSetupRegister_t;

typedef union {
	struct {
		uint16_t MMDRWIndexData;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYMMDData_t;

typedef union {
	struct {
		uint16_t :12;
		uint16_t _1000BASETHalfDuplex:1;
		uint16_t _1000BASETFullDuplex:1;
		uint16_t _1000BASEXHalfDuplex:1;
		uint16_t _1000BASEXFullDuplex:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYExtendedStatus_t;

typedef union {
	struct {
		uint16_t :1;
		uint16_t :1;
		uint16_t :6;
		uint16_t RemoteLoopBack:1;
		uint16_t :7;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYRemoteLoopback_t;

typedef union {
	struct {
		uint16_t :8;
		uint16_t CableDiagnosticStatus:2;
		uint16_t :2;
		uint16_t CableDiagnosticTestPair:2;
		uint16_t :1;
		uint16_t CableDiagnosticTestEnable:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYLinkMD_t;

typedef union {
	struct {
		uint16_t _100BASETXLinkStatus:1;
		uint16_t _1000BASETLinkStatus:1;
		uint16_t :14;
	} bit;
	uint16_t reg;
} KSZ9893_PortNDigitalPMAPCSStatus_t;

typedef union {
	struct {
		uint16_t RXERCounter;
	} bit;
	uint16_t reg;
} KSZ9893_PortNRXERCount_t;

typedef union {
	struct {
		uint16_t LinkUpInterrupt:1;
		uint16_t RemoteFaultInterrupt:1;
		uint16_t LinkDownInterrupt:1;
		uint16_t LinkPartnerAcknowledgeInterrupt:1;
		uint16_t ParallelDetectFaultInterrupt:1;
		uint16_t PageReceivedInterrupt:1;
		uint16_t ReceiveErrorInterrupt:1;
		uint16_t JabberInterrupt:1;
		uint16_t LinkUpInterruptEnable:1;
		uint16_t RemoteFaultInterruptEnable:1;
		uint16_t LinkDownInterruptEnable:1;
		uint16_t LinkPartnerAcknowledgeInterruptEnable:1;
		uint16_t ParallelDetectFaultInterruptEnable:1;
		uint16_t PageReceivedInterruptEnable:1;
		uint16_t ReceiveErrorInterruptEnable:1;
		uint16_t JabberInterruptEnable:1;
	} bit;
	uint16_t reg;
} KSZ9893_PortNInterruptControlStatus_t;

typedef union {
	struct {
		uint16_t :6;
		uint16_t SwapOff:1;
		uint16_t MDISet:1;
		uint16_t :8;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYAutoMDIMDIX_t;

typedef union {
	struct {
		uint16_t :1;
		uint16_t :1;
		uint16_t _1000BASETMasterSlaveStatus:1;
		uint16_t DuplexStatus:1;
		uint16_t SpeedStatus10BASET:1;
		uint16_t SpeedStatus100BASETX:1;
		uint16_t SpeedStatus1000BASET:1;
		uint16_t :2;
		uint16_t EnableJabber:1;
		uint16_t :2;
		uint16_t :4;
	} bit;
	uint16_t reg;
} KSZ9893_PortNPHYControl_t;

typedef union {
	struct {
		uint8_t :3;
		uint8_t MACPortReceiveFlowControlEnable:1;
		uint8_t MACPortSpeed10_100:1;
		uint8_t MACPortTransmitFlowControlEnable:1;
		uint8_t MACPortDuplex:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_Port3XMIIPortControl0_t;

typedef union {
	struct {
		uint8_t PortInterfaceTypeSelect:2;
		uint8_t MII_RMII_RGMII_Modes:1;
		uint8_t RGMIIEgressInternalDelay:1;
		uint8_t RGMIIIngressInternalDelay:1;
		uint8_t :1;
		uint8_t PortSpeed1000:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_Port3XMIIPortControl1_t;

typedef union {
	struct {
		uint8_t RGMIIIBSLinkStatus:1;
		uint8_t RGMIIIBSRX_CLKClockSpeed:2;
		uint8_t RGMIIIBSDuplexStatus:1;
		uint8_t :4;
	} bit;
	uint8_t reg;
} KSZ9893_Port3XMIIPortControl3_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t BroadcaseStormProtectionEnable:1;
		uint8_t :1;
		uint8_t :1;
		uint8_t :4;
	} bit;
	uint8_t reg;
} KSZ9893_PortNMACControl0_t;

typedef union {
	struct {
		uint8_t PassAllFrames:1;
		uint8_t :2;
		uint8_t BackPressureEnable:1;
		uint8_t :2;
		uint8_t :1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNMACControl1_t;

typedef union {
	struct {
		uint8_t CountPreambleBytes:1;
		uint8_t CountIGFBytes:1;
		uint8_t IngressLimitMode:2;
		uint8_t IngressRateLimitFlowControlEnable:1;
		uint8_t IngressPPSMode:1;
		uint8_t PortOrPriorityBaseIngressRateLimiting:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNIngressRateLimitControl_t;

typedef union {
	struct {
		uint8_t IngressDataRateLimitForPriorityXFrames:7;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNPriorityXIngressLimitControl_t;

typedef union {
	struct {
		uint8_t EgressDataRateLimitForQueueXFrames:7;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNQueueXEgressLimitControl_t;

typedef union {
	struct {
		uint32_t MIBCounterValue35_32:4;
		uint32_t :12;
		uint32_t MIBIndex:8;
		uint32_t MIBFlushAndFreezeEnable:1;
		uint32_t MIBReadEnableCountValid:1;
		uint32_t :5;
		uint32_t MIBCounterOverflowIndication:1;
	} bit;
	uint32_t reg;
} KSZ9893_PortNMIBControlAndStatus_t;

typedef union {
	struct {
		uint32_t MIBCounterValue31_0;
	} bit;
	uint32_t reg;
} KSZ9893_PortNMIBData_t;

typedef union {
	struct {
		uint8_t FirstRuleNumber:4;
		uint8_t :4;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess0_t;

typedef union {
	struct {
		uint8_t CompareEqual:1;
		uint8_t SourceDestination:1;
		uint8_t Enable:2;
		uint8_t Mode:2;
		uint8_t :2;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess1_t;

typedef union {
	struct {
		uint8_t MACAddress47_40;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess2_t;

typedef union {
	struct {
		uint8_t MACAddress39_32;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess3_t;

typedef union {
	struct {
		uint8_t MACAddress31_24;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess4_t;

typedef union {
	struct {
		uint8_t MACAddress23_16;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess5_t;

typedef union {
	struct {
		uint8_t MACAddress15_8;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess6_t;

typedef union {
	struct {
		uint8_t MACAddress7_0;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess7_t;

typedef union {
	struct {
		uint8_t EtherType15_8;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess8_t;

typedef union {
	struct {
		uint8_t EtherType7_0;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccess9_t;

typedef union {
	struct {
		uint8_t RemarkPriority2_1:2;
		uint8_t RemarkPriorityEnable:1;
		uint8_t Priority:3;
		uint8_t PriorityMode:2;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessA_t;

typedef union {
	struct {
		uint8_t :5;
		uint8_t MapMode:2;
		uint8_t RemarkPriority0:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessB_t;

typedef union {
	struct {
		uint8_t RESERVED;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessC_t;

typedef union {
	struct {
		uint8_t Port1ForwardMap:1;
		uint8_t Port2ForwardMap:1;
		uint8_t Port3ForwardMap:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessD_t;

typedef union {
	struct {
		uint8_t Ruleset15_8;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessE_t;

typedef union {
	struct {
		uint8_t Ruleset7_0;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessF_t;

typedef union {
	struct {
		uint8_t ACLAccess7Enable:1;
		uint8_t ACLAccess6Enable:1;
		uint8_t ACLAccess5Enable:1;
		uint8_t ACLAccess4Enable:1;
		uint8_t ACLAccess3Enable:1;
		uint8_t ACLAccess2Enable:1;
		uint8_t ACLAccess1Enable:1;
		uint8_t ACLAccess0Enable:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLByteEnableMSB_t;

typedef union {
	struct {
		uint8_t ACLAccessFEnable:1;
		uint8_t ACLAccessEEnable:1;
		uint8_t ACLAccessDEnable:1;
		uint8_t ACLAccessCEnable:1;
		uint8_t ACLAccessBEnable:1;
		uint8_t ACLAccessAEnable:1;
		uint8_t ACLAccess9Enable:1;
		uint8_t ACLAccess8Enable:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLByteEnableLSB_t;

typedef union {
	struct {
		uint8_t ACLIndex:4;
		uint8_t WriteRead:1;
		uint8_t ReadStatus:1;
		uint8_t WriteStatus:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNACLAccessControl0_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t SnifferPort:1;
		uint8_t :3;
		uint8_t TransmitSniff:1;
		uint8_t ReceiveSniff:1;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNMirroringControl_t;

typedef union {
	struct {
		uint8_t ACLPriorityClassification:1;
		uint8_t DiffservPriorityClassification:1;
		uint8_t _8021pPriorityClassification:1;
		uint8_t VLANPriorityClassficiation:1;
		uint8_t MACAddressPriorityClassification:1;
		uint8_t :1;
		uint8_t ORedPriority:1;
		uint8_t HighestPriority:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNPriorityControl_t;

typedef union {
	struct {
		uint8_t PortDefaultPriorityClassification:3;
		uint8_t DiscardTaggedPackets:1;
		uint8_t DiscardUntaggedPackets:1;
		uint8_t :2;
		uint8_t UserPriorityCeiling:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNIngressMACControl_t;

typedef union {
	struct {
		uint8_t AuthenticationMode:2;
		uint8_t AccessControlListEnable:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNAuthenticationControl_t;

typedef union {
	struct {
		uint32_t QueuePointer:2;
		uint32_t :14;
		uint32_t PortIndex:3;
		uint32_t :13;
	} bit;
	uint32_t reg;
} KSZ9893_PortNPointer_t;

typedef union {
	struct {
		uint32_t RegeneratedPriorityValueForPriority0:4;
		uint32_t RegeneratedPriorityValueForPriority1:4;
		uint32_t RegeneratedPriorityValueForPriority2:4;
		uint32_t RegeneratedPriorityValueForPriority3:4;
		uint32_t RegeneratedPriorityValueForPriority4:4;
		uint32_t RegeneratedPriorityValueForPriority5:4;
		uint32_t RegeneratedPriorityValueForPriority6:4;
		uint32_t RegeneratedPriorityValueForPriority7:4;
	} bit;
	uint32_t reg;
} KSZ9893_PortNPriorityToQueueMapping_t;

typedef union {
	struct {
		uint32_t PoliceEnable:1;
		uint32_t PoliceMode:1;
		uint32_t DropSRP:1;
		uint32_t ColorRemapEnable:1;
		uint32_t ColorMarkEnable:1;
		uint32_t NONDSCP_COLOR:2;
		uint32_t PortBasedPolicing:1;
		uint32_t PacketType:2;
		uint32_t DropAll:1;
		uint32_t DroppedColor:1;
		uint32_t :20;
	} bit;
	uint32_t reg;
} KSZ9893_PortNPoliceControl_t;

typedef union {
	struct {
		uint32_t QueueIndex:2;
		uint32_t :30;
	} bit;
	uint32_t reg;
} KSZ9893_PortNTransmitQueueIndex_t;

typedef union {
	struct {
		uint32_t PortVIDReplacement:1;
		uint32_t :31;
	} bit;
	uint32_t reg;
} KSZ9893_PortNTransmitQueuePVID_t;

typedef union {
	struct {
		uint8_t :6;
		uint8_t SchedulerMode:2;
	} bit;
	uint8_t reg;
} KSZ9893_PortNTransmitQueueControl0_t;

typedef union {
	struct {
		uint8_t QueueWeightforWRRScheduling:7;
		uint8_t :1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNTransmitQueueControl1_t;

typedef union {
	struct {
		uint32_t TBD;
	} bit;
	uint32_t reg;
} KSZ9893_PortNControl0_t;

typedef union {
	struct {
		uint32_t Port1VLANMembership:1;
		uint32_t Port2VLANMembership:1;
		uint32_t Port3VLANMembership:1;
		uint32_t :29;
	} bit;
	uint32_t reg;
} KSZ9893_PortNControl1_t;

typedef union {
	struct {
		uint8_t :1;
		uint8_t :1;
		uint8_t :1;
		uint8_t SelfAddressFiltering_PortEnable:1;
		uint8_t MACBased8021XEnable:1;
		uint8_t DiscardNonPVIDPacket:1;
		uint8_t IngressVLANFiltering:1;
		uint8_t NullVIDLoopupEnable:1;
	} bit;
	uint8_t reg;
} KSZ9893_PortNControl2_t;

typedef union {
	struct {
		uint8_t MSTPPointer:3;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNMSTPPointer_t;

typedef union {
	struct {
		uint8_t PortLearningDisable:1;
		uint8_t PortReceiveEnable:1;
		uint8_t PortTransmitEnable:1;
		uint8_t :5;
	} bit;
	uint8_t reg;
} KSZ9893_PortNMSTPState_t;

#endif /* KSZ9893_REGISTERS_H_ */