/*
 * KSZ8091_registers.h
 *
 * Created: 2019-07-29 3:28:48 PM
 *  Author: Wizz
 */ 


#ifndef KSZ8091_REGISTERS_H_
#define KSZ8091_REGISTERS_H_

// Vendor Specific Registers
#define KSZ8091_DigitalReservedControl				0x10
#define KSZ8091_AFEControl1							0x11
#define KSZ8091_AFEControl4							0x13
#define KSZ8091_RXERCounter							0x15
#define KSZ8091_OperationModeStrapOverride			0x16
#define KSZ8091_OperationModeStrapStatus			0x17
#define KSZ8091_ExpandedControl						0x18
#define KSZ8091_InterruptControlStatus				0x1B
#define KSZ8091_LinkMDCableDiagnostic				0x1D
#define KSZ8091_PHYControl1							0x1E
#define KSZ8091_PHYControl2							0x1F

// MMD Indirect Registers
#define KSZ8091_PMAPMD_DeviceAddress				0x01
#define KSZ8091_PMAPMDControl1						0x00
#define KSZ8091_PMAPMDStatus1						0x01
#define KSZ8091_EEE_DeviceAddress					0x07
#define KSZ8091_EEEAdvertisement					0x3C
#define KSZ8091_EEELinkPartnerAdvertisement			0x3D
#define KSZ8091_DSP_DeviceAddress					0x1C
#define KSZ8091_DSP10BASETControl					0x04
#define KSZ8091_WakeOnLAN_DeviceAddress				0x1F
#define KSZ8091_WakeOnLANControl					0x00
#define KSZ8091_WakeOnLANCustPacketType0Mask0		0x01
#define KSZ8091_WakeOnLANCustPacketType0Mask1		0x02
#define KSZ8091_WakeOnLANCustPacketType0Mask2		0x03
#define KSZ8091_WakeOnLANCustPacketType0Mask3		0x04
#define KSZ8091_WakeOnLANCustPacketType0MaskCRC0	0x05
#define KSZ8091_WakeOnLANCustPacketType0MaskCRC1	0x06
#define KSZ8091_WakeOnLANCustPacketType1Mask0		0x07
#define KSZ8091_WakeOnLANCustPacketType1Mask1		0x08
#define KSZ8091_WakeOnLANCustPacketType1Mask2		0x09
#define KSZ8091_WakeOnLANCustPacketType1Mask3		0x0A
#define KSZ8091_WakeOnLANCustPacketType1MaskCRC0	0x0B
#define KSZ8091_WakeOnLANCustPacketType1MaskCRC1	0x0C
#define KSZ8091_WakeOnLANCustPacketType2Mask0		0x0D
#define KSZ8091_WakeOnLANCustPacketType2Mask1		0x0E
#define KSZ8091_WakeOnLANCustPacketType2Mask2		0x0F
#define KSZ8091_WakeOnLANCustPacketType2Mask3		0x10
#define KSZ8091_WakeOnLANCustPacketType2MaskCRC0	0x11
#define KSZ8091_WakeOnLANCustPacketType2MaskCRC1	0x12
#define KSZ8091_WakeOnLANCustPacketType3Mask0		0x13
#define KSZ8091_WakeOnLANCustPacketType3Mask1		0x14
#define KSZ8091_WakeOnLANCustPacketType3Mask2		0x15
#define KSZ8091_WakeOnLANCustPacketType3Mask3		0x16
#define KSZ8091_WakeOnLANCustPacketType3MaskCRC0	0x17
#define KSZ8091_WakeOnLANCustPacketType3MaskCRC1	0x18
#define KSZ8091_WakeOnLANMagicPacketMACDA0			0x19
#define KSZ8091_WakeOnLANMagicPacketMACDA1			0x1A
#define KSZ8091_WakeOnLANMagicPacketMACDA2			0x1B

typedef union {
	struct {
		uint16_t :4;
		uint16_t PPLOff:1;
		uint16_t :11;
	} bit;
	uint16_t reg;
} KSZ8091_DigitalReservedControl_t;

typedef union {
	struct {
		uint16_t :5;
		uint16_t SlowOscillatorEnable:1;
		uint16_t :10;
	} bit;
	uint16_t reg;
} KSZ8091_AFEControl1_t;

typedef union {
	struct {
		uint16_t :4;
		uint16_t b10BaseTeMode:1;
		uint16_t :11;
	} bit;
	uint16_t reg;
} KSZ8091_AFEControl4_t;

typedef uint16_t KSZ8091_RXERCounter_t;

typedef union {
	struct {
		uint16_t :1;
		uint16_t RMIIOverride:1;
		uint16_t :3;
		uint16_t NANDTreeOverride:1;
		uint16_t RMIIBtoBOverride:1;
		uint16_t :2;
		uint16_t BCASTOFFOverride:1;
		uint16_t :1;
		uint16_t :4;
		uint16_t PMEEnable:1;
	} bit;
	uint16_t reg;
} KSZ8091_OperationModeStrapOverride_t;

typedef union {
	struct {
		uint16_t :1;
		uint16_t RMIIStrapInStatus:1;
		uint16_t :11;
		uint16_t PHYADStrainStatus:2;
		uint16_t 1;
	} bit;
	uint16_t reg;
} KSZ8091_OperationModeStrapStatus_t;

typedef union {
	struct {
		uint16_t :11;
		uint16_t EDPDDisabled:1;
		uint16_t :4;
	} bit;
	uint16_t reg;
} KSZ8091_ExpandedControl_t;

typedef union {
	struct {
		uint16_t LinkUpInterrupt:1;
		uint16_t RemoteFaultInterrupt:1;
		uint16_t LinkDownInterrupt:1;
		uint16_t LinkPartnerAcknowledgeInterrupt:1;
		uint16_t ParallelDetectFaultInterrupt:1;
		uint16_t PageReceiveInterrupt:1;
		uint16_t ReceiveErrorInterrupt:1;
		uint16_t JabberInterrupt:1;
		uint16_t LinkUpInterruptEnable:1;
		uint16_t RemoteFaultInterruptEnable:1;
		uint16_t LinkDownInterruptEnable:1;
		uint16_t LinkPartnerAcknowledgeInterruptEnable:1;
		uint16_t ParallelDetectFaultInterruptEnable:1;
		uint16_t PageReceiveInterruptEnable:1;
		uint16_t ReceiveErrorInterruptEnable:1;
		uint16_t JabberInterruptEnable:1;
	} bit;
	uint16_t reg;
} KSZ8091_InterruptControlStatus_t;

typedef union {
	struct {
		uint16_t DistanceToFault:9;
		uint16_t :3;
		uint16_t ShortCableIndicator:1;
		uint16_t CableDiagnosticTestResult:2;
		uint16_t CableDiagnosticTestEnable:1;
	} bit;
	uint16_t reg;
} KSZ8091_LinkMDCableDiagnostic_t;

typedef union {
	struct {
		uint16_t OperationModeIndication:3;
		uint16_t PHYIsolate:1;
		uint16_t EnergyDetect:1;
		uint16_t MDIMDIXState:1;
		uint16_t :1;
		uint16_t PolarityStatus:1;
		uint16_t LinkStatus:1;
		uint16_t EnablePauseFlowControl:1;
		uint16_t :6;
	} bit;
	uint16_t reg;
} KSZ8091_PHYControl1_t;

typedef union {
	struct {
		uint16_t DisableDataScrambling:1;
		uint16_t :1;
		uint16_t RemoteLoopback:1;
		uint16_t DisableTransmitter:1;
		uint16_t LEDMode:2;
		uint16_t :1;
		uint16_t RMIIReferenceClockSelect:1;
		uint16_t EnableJabber:1;
		uint16_t InterruptLevel:1;
		uint16_t PowerSaving:1;
		uint16_t ForceLink:1;
		uint16_t :1;
		uint16_t PairSwapDisable:1;
		uint16_t MDIMDIXSelect:1;
		uint16_t HP_MDIX:1;
	} bit;
	uint16_t reg;
} KSZ8091_PHYControl2_t;

#endif /* KSZ8091_REGISTERS_H_ */