/*
 * CortexM4_SCBRegisters.h
 *
 * Created: 2020-07-09 10:31:09 AM
 *  Author: charl
 */ 


#ifndef CORTEXM4_SCBREGISTERS_H_
#define CORTEXM4_SCBREGISTERS_H_

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t Revision:4;
		uint32_t PartNo:12;
		uint32_t Constant:4;
		uint32_t Variant:4;
		uint32_t Implementer:8;
	} bit;
	uint32_t reg;
} CortexM4_SCB_CPUID_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t VECTACTIVE:9;
		uint32_t :2;
		uint32_t RETOBASE:1;
		uint32_t VECTPENDING:10;
		uint32_t ISRPENDING:1;
		uint32_t :2;
		uint32_t PENDSTCLR:1;
		uint32_t PENDSTSET:1;
		uint32_t PENDSVCLR:1;
		uint32_t PENDSVSET:1;
		uint32_t :2;
		uint32_t NMIPENDSET:1;
	} bit;
	uint32_t reg;
} CortexM4_SCB_ICSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t :9;
		uint32_t TABLEOFF:21;
		uint32_t :2;
	} bit;
	uint32_t reg;
} CortexM4_SCB_VTOR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t VECTRESET:1;
		uint32_t VECTCLRACTIVE:1;
		uint32_t SYSRESETREQ:1;
		uint32_t :5;
		uint32_t PRIGROUP:3;
		uint32_t :4;
		uint32_t ENDIANESS:1;
		uint32_t VECTKEY:16;
	} bit;
	uint32_t reg;
} CortexM4_SCB_AIRCR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t :1;
		uint32_t SLEEPONEXIT:1;
		uint32_t SLEEPDEEP:1;
		uint32_t :1;
		uint32_t SEVONPEND:1;
		uint32_t :27;
	} bit;
	uint32_t reg;	
} CortexM4_SCB_SCR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t NONBASETHRDENA:1;
		uint32_t USERSETMPEND:1;
		uint32_t :1;
		uint32_t UNALIGN_TRP:1;
		uint32_t DIV_0_TRP:1;
		uint32_t :3;
		uint32_t BFHFNIGN:1;
		uint32_t STKALIGN:1;
		uint32_t :22;
	} bit;
	uint32_t reg;
} CortexM4_SCB_CCR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t MEMFAULTACT:1;		// Memory Management Fault exception active bit, reads as 1 if exception is active.
		uint32_t BUSFAULTACT:1;		// BusFault exception active bit, reads as 1 if exception is active.
		uint32_t :1;
		uint32_t USGFAULTACT:1;		// UsageFault exception active bit, reads as 1 if exception is active.
		uint32_t :3;
		uint32_t SVCALLACT:1;		
		uint32_t MONITORACT:1;		
		uint32_t :1;
		uint32_t PENDSVACT:1;
		uint32_t SYSTICKACT:1;
		uint32_t USGFAULTPENDED:1;	// UsageFault exception pending bit, reads as 1 if exception is pending.
		uint32_t MEMFAULTPENDED:1;	// Memory Management Fault exception pending bit, reads as 1 if exception is pending.
		uint32_t BUSFAULTPENDED:1;	// BusFault exception pending bit, reads as 1 if exception is pending.
		uint32_t SVCALLPENDED:1;
		uint32_t MEMFAULTENA:1;		// Memory Management Fault exception enable bit, set to 1 to enable; set to 0 to disable.
		uint32_t BUSFAULTENA:1;		// BusFault exception enable bit, set to 1 to enable; set to 0 to disable.
		uint32_t USGFAULTENA:1;		// UsageFault exception enable bit, set to 1 to enable; set to 0 to disable.
		uint32_t :13;
	} bit;
	uint32_t reg;
} CortexM4_SCB_SHCSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint8_t IACCVIOL:1;		/* Instruction Access Violation Flag:
									The processor attempted an instruction fetch from a location that does not permit execution.
									The PC value stacked for the exception return points to the faulting instruction. The processor
									has not written a fault address to the MMFAR. This fault condition occurs on any attempt of
									instruction fetches to an XN (eXecute Never) region, even when the MPU is disabled or not
									present. Potential reasons:
										a) Branch to regions that are not defined in the MPU or defined as non-executable.
										b) Invalid return due to corrupted stack content.										c) Incorrect entry in the exception vector table.*/
		uint8_t DACCVIOL:1;		/* Data Access Violation Flag:
									The processor attempted a load or store at a location that does not permit the operation.
									The PC value stacked for the exception return points to the faulting instruction. The processor
									has loaded the MMFAR with the address of the attempted access. */
		uint8_t :1;
		uint8_t MUNSTKERR:1;	/* MemManage Fault on Unstacking for a Return from Exception:
									Unstacking for an exception return has caused one or more access violations.
									This fault is chained to the handler which means that the original return stack is still present.
									The processor has not adjusted the SP from the failing return, and has not performed a new
									save. The processor has not written a fault address to the MMFAR. Potential reasons:
										a) Stack pointer is corrupted
										b) MPU region for the stack changed during execution of the exception handler. */
		uint8_t MSTKERR:1;		/* MemManage Fault on Stacking for Exception Entry:
									Stacking for an exception entry has caused one or more access violations.
									The SP is still adjusted but the values in the context area on the stack might be incorrect. The
									processor has not written a fault address to the MMFAR. Potential reasons:
										a) Stack pointer is corrupted or not initialized
										b) Stack is reaching a region not defined by the MPU as read/write memory. */
		uint8_t MLSPERR:1;		/* MemManage Fault during Floating Point lazy state preservation (only Cortex-M4 with FPU):
									Fault occurred during floating-point lazy state preservation.*/
		uint8_t :1;
		uint8_t MMARVALID:1;	/* MemManage Fault Address Register (MMFAR) valid flag:
									SCB->MMFAR holds a valid fault address.
									If a MemManage fault occurs and is escalated to a HardFault because of priority, the HardFault
									handler must set this bit to 0. This prevents problems on return to a stacked active MemManage
									fault handler whose SCB->MMFAR value has been overwritten. */
	} bit;
	uint8_t reg;
} CortexM4_SCB_CFSR_MMFSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint8_t IBUSERR:1;		/* Instruction bus error. Records whether a BusFault on an instruction pre-fetch has occurred.
									The processor detects the instruction bus error on pre-fetching an instruction, but it sets the
									AN209 – Using Cortex-M3/M4/M7 Fault Exceptions Copyright © 2017 ARM Ltd. All rights reserved
									feedback@keil.com www.keil.com/appnotes/docs/apnt_209.asp
									IBUSERR flag to 1 only if it attempts to issue the faulting instruction. When the processor sets
									this bit, it does not write a fault address to BFAR. Potential reasons:
										a) Branch to invalid memory regions for example caused by incorrect function pointers.
										b) Invalid return due to corrupted stack pointer or stack content.
										c) Incorrect entry in the exception vector table. */
		uint8_t PRECISERR:1;	/* Precise Data Bus Error:
									A data bus error has occurred, and the PC value stacked for the exception return points to
									the instruction that caused the fault.
									When the processor sets this bit, it writes the faulting address to BFAR. */
		uint8_t IMPRECISERR:1;	/*  Imprecise Data Bus Error:
									A data bus error has occurred, but the return address in the stack frame is not related to the
									instruction that caused the error.
									When the processor sets this bit it does not write a fault address to BFAR. This is an
									asynchronous fault. Therefore, if it is detected when the priority of the current process is higher
									than the BusFault priority, the BusFault becomes pending and becomes active only when the
									processor returns from all higher priority processes. If a precise fault occurs before the
									processor enters the handler for the imprecise BusFault, the handler detects both IMPRECISERR
									set to 1 and one of the precise fault status bits set to 1. */
		uint8_t UNSTKERR:1;		/* BusFault on Unstacking for a Return from Exception:
									Unstack for an exception return has caused one or more BusFaults.
									This fault is chained to the handler. This means that when the processor sets this bit, the original
									return stack is still present. The processor does not adjust the SP from the failing return, does
									not performed a new save, and does not write a fault address to BFAR. */
		uint8_t STRERR:1;		/* BusFault on Stacking for Exception Entry:
									Stacking for an exception entry has caused one or more BusFaults.
									When the processor sets this bit, the SP is still adjusted but the values in the context area on the
									stack might be incorrect. The processor does not write a fault address to the BFAR. Potential reasons:
										a) Stack pointer is corrupted or not initialized
										b) Stack is reaching an undefined memory region. */
		uint8_t LSPERR:1;		/* BusFault During Floating Point lazy state preservation (only when FPU present):
									Fault occurred during floating-point lazy state preservation. */
		uint8_t :1;
		uint8_t BFARVALID:1;	/* BusFault Address Register (BFAR) Valid Flag:									BFAR holds a valid fault address.
									The processor sets this bit after a BusFault where the address is known. Other faults can set this
									bit to 0, such as a MemManage fault occurring later. If a BusFault occurs and is escalated to a
									HardFault because of priority, the HardFault handler must set this bit to 0. This prevents
									problems if returning to a stacked active BusFault handler who's BFAR value has been
									overwritten. */
	} bit;
	uint8_t reg;
} CortexM4_SCB_CFSR_BFSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint16_t UNDEFINSTR:1;	/* Undefined Instruction
									The processor has attempted to execute an undefined instruction.
									When this bit is set, the PC value stacked for the exception return points to the undefined
									instruction. An undefined instruction is an instruction that the processor cannot decode.
									Potential reasons:
										a) Use of instructions not supported in the Cortex-M device.
										b) Bad or corrupted memory contents. */
		uint16_t INVSTATE:1;	/* Invalid State
									The processor has attempted to execute an instruction that makes illegal use of the
									Execution Program Status Register (EPSR). When this bit is set, the PC value stacked
									for the exception return points to the instruction that	attempted the illegal use of
									the EPSR. Potential reasons:
										a) Loading a branch target address to PC with LSB=0.
										b) Stacked PSR corrupted during exception or interrupt handling.
										c) Vector table contains a vector address with LSB=0. */
		uint16_t INVPC:1;		/* Invalid PC load UsageFault, caused by an invalid EXC_RETURN value:
									The processor has attempted to load an illegal EXC_RETURN value to the PC as a result of an
									invalid context switch.	When this bit is set, the PC value stacked for the exception return
									points to the instruction that tried to perform the illegal load of the PC. Potential reasons:
										a) Invalid return due to corrupted stack pointer, link register (LR), or stack content.
										b) ICI/IT bit in PSR invalid for an instruction. */
		uint16_t NOCP:1;		/* No co-processor. The processor does not support co-processor instructions:
									The processor has attempted to access a co-processor that does not exist. */
		uint16_t :4;
		uint16_t UNALIGNED:1;	/* Unaligned access UsageFault:
									The processor has made an unaligned memory access. Enable trapping of unaligned accesses
									by setting the UNALIGN_TRP bit in the CCR. Unaligned LDM, STM, LDRD, and STRD instructions
									always fault irrespective of the setting of UNALIGN_TRP bit. */
		uint16_t DIVBYZERO:1;	/* Divide by zero UsageFault:
									The processor has executed an SDIV or UDIV instruction with a divisor of 0.
									When the processor sets this bit to 1, the PC value stacked for the exception return points to the
									instruction that performed the divide by zero. Enable trapping of divide by zero by setting the
									DIV_0_TRP bit in the CCR to 1. */
		uint16_t :6;
	} bit;
	uint16_t reg;
} CortexM4_SCB_CFSR_UFSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		CortexM4_SCB_CFSR_MMFSR_t MMFSR;
		CortexM4_SCB_CFSR_BFSR_t BFSR;
		CortexM4_SCB_CFSR_UFSR_t UFSR;
	};
	uint32_t reg;
} CortexM4_SCB_CFSR_t;

typedef union {
	struct __attribute__((__packed__)) {
		uint32_t :1;
		uint32_t VECTTBL:1;		/* Indicates a Bus Fault on a vector table read during exception processing:
								When this bit is set, the PC value stacked for the exception return points to the instruction
								that was preempted by the exception. This error is always a Hard Fault */
		uint32_t :28;
		uint32_t FORCED:1;		/* Indicates a forced Hard Fault, generated by escalation of a fault with configurable priority that
								cannot be handled, either because of priority or because it is disabled:
								When this bit is set, the Hard Fault handler must read the other fault status registers to find
								the cause of the fault. */
		uint32_t DEBUG_VT:1;	/* Reserved for Debug use. When writing to the register you must write 0 to this bit, otherwise
								behavior is unpredictable. */
	} bit;
	uint32_t reg;
} CortexM4_SCB_HFSR_t;

typedef struct
{
	CortexM4_SCB_CPUID_t CPUID;       /*!< Offset: 0x000 (R/ )  CPUID Base Register */
	CortexM4_SCB_ICSR_t ICSR;        /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
	CortexM4_SCB_VTOR_t VTOR;        /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
	CortexM4_SCB_AIRCR_t AIRCR;      /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
	CortexM4_SCB_SCR_t SCR;          /*!< Offset: 0x010 (R/W)  System Control Register */
	CortexM4_SCB_CCR_t CCR;          /*!< Offset: 0x014 (R/W)  Configuration Control Register */
	uint8_t  SHP[12U];               /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
	CortexM4_SCB_SHCSR_t SHCSR;      /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
	CortexM4_SCB_CFSR_t CFSR;			/*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
	CortexM4_SCB_HFSR_t HFSR;			/*!< Offset: 0x02C (R/W)  HardFault Status Register */
	uint32_t DFSR;                   /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
	uint32_t MMFAR;                  /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
	uint32_t BFAR;                   /*!< Offset: 0x038 (R/W)  BusFault Address Register */
	uint32_t AFSR;                   /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
	uint32_t PFR[2U];                /*!< Offset: 0x040 (R/ )  Processor Feature Register */
	uint32_t DFR;                    /*!< Offset: 0x048 (R/ )  Debug Feature Register */
	uint32_t ADR;                    /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
	uint32_t MMFR[4U];               /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
	uint32_t ISAR[5U];               /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register */
	uint32_t RESERVED0[5U];
	uint32_t CPACR;                  /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register */
} CortexM4_SCB_t;

#endif /* CORTEXM4_SCBREGISTERS_H_ */