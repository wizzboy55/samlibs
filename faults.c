/*
 * faults.c
 *
 * Created: 2020-07-09 10:23:12 AM
 *  Author: charl
 */ 

#include "faults.h"

#include "FreeRTOS.h"
#include "task.h"

#include "CortexM4_SCBRegisters.h"

#include "hpl_pm_base.h"

#include "utilities.h"
#include "debug_interface.h"

void vEnableAllFaults(void) {
	#if defined(SAME54) || defined(SAME53)
	CortexM4_SCB_t* pSCB = (CortexM4_SCB_t*)SCB;
	pSCB->SHCSR.bit.BUSFAULTENA = 1;
	pSCB->SHCSR.bit.USGFAULTENA = 1;
	pSCB->SHCSR.bit.MEMFAULTENA = 1;
	pSCB->CCR.bit.DIV_0_TRP = 1;
	pSCB->CCR.bit.UNALIGN_TRP = 1;
	#elif defined (SAMD20)
	
	#endif
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {

	DEBUG_printf( ("StackOverflow! Task: %s\tHandle: %d\n", pcTaskGetName(xTaskGetCurrentTaskHandle()), (uint32_t)xTaskGetCurrentTaskHandle()) );

	#ifdef DEBUG
	for(;;){}
	#else
	utils_WatchDogReset();
	#endif
}

void vApplicationMallocFailedHook(void) {

	DEBUG_printf( ("Malloc Fail!\n") );

	#ifdef DEBUG
	for(;;){}
	#else
	utils_WatchDogReset();
	#endif
}

void vAssertCalled(const char *pcFileName, unsigned long ulLine) {

	DEBUG_printf( ("ASSERT! F: %s L: %lu\n", pcFileName, ulLine) );

	#ifdef DEBUG
	for(;;){}
	#else
	utils_WatchDogReset();
	#endif
}

void vPrintUsageFaultCortexM4(void) {
	CortexM4_SCB_t* pSCB = (CortexM4_SCB_t*)SCB;
	DEBUG_printf( ("Usage Fault.\n") );
	if(pSCB->CFSR.UFSR.bit.UNDEFINSTR) {
		DEBUG_printf( ("Undefined Instruction Fault.\n") );
	}
	if(pSCB->CFSR.UFSR.bit.INVSTATE) {
		DEBUG_printf( ("Invalid State Fault.\n") );
	}
	if(pSCB->CFSR.UFSR.bit.INVPC) {
		DEBUG_printf( ("Invalid PC Load Fault.\n") );
	}
	if(pSCB->CFSR.UFSR.bit.NOCP) {
		DEBUG_printf( ("No Coprocessor Fault.\n") );
	}
	if(pSCB->CFSR.UFSR.bit.UNALIGNED) {
		DEBUG_printf( ("Unaligned Access Fault.\n") );
	}
	if(pSCB->CFSR.UFSR.bit.DIVBYZERO) {
		DEBUG_printf( ("Division by Zero Fault.\n") );
	}
}

void vPrintMemManagementFaultCortexM4(void) {
	CortexM4_SCB_t* pSCB = (CortexM4_SCB_t*)SCB;
	DEBUG_printf( ("Memory Management Fault.\n") );
	if(pSCB->CFSR.MMFSR.bit.IACCVIOL) {
		DEBUG_printf( ("Instruction Access Violation Fault.\n") );
	}
	if(pSCB->CFSR.MMFSR.bit.DACCVIOL) {
		DEBUG_printf( ("Data Access Violation Fault.\n") );
	}
	if(pSCB->CFSR.MMFSR.bit.MUNSTKERR) {
		DEBUG_printf( ("Unstacking for a Return From Exception Fault.\n") );
	}
	if(pSCB->CFSR.MMFSR.bit.MSTKERR) {
		DEBUG_printf( ("Stacking for an Exception Entry Fault.\n") );
	}
	if(pSCB->CFSR.MMFSR.bit.MLSPERR) {
		DEBUG_printf( ("Floating-Point Lazy State Preservation Fault.\n") );
	}
	if(pSCB->CFSR.MMFSR.bit.MMARVALID) {
		DEBUG_printf( ("Fault Address: %08X.\n", pSCB->MMFAR) );
	}
}

void vCheckFaultStatusRegisterCortexM4(void) {
	CortexM4_SCB_t* pSCB = (CortexM4_SCB_t*)SCB;
	if(pSCB->HFSR.bit.FORCED) {
		DEBUG_printf( ("Hard Fault is Forced.\n") );
		if(pSCB->CFSR.BFSR.reg) {
			DEBUG_printf( ("Bus Fault.\n") );
		}
		if(pSCB->CFSR.MMFSR.reg) {
			vPrintMemManagementFaultCortexM4();
		}
		if(pSCB->CFSR.UFSR.reg) {
			vPrintUsageFaultCortexM4();
		}
	} else {
		DEBUG_printf( ("Hard Fault is not forced.\n") );
	}
}

void prvCheckFaultStatusRegisterCortexM0(void) {

}

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
	/* These are volatile to try and prevent the compiler/linker optimizing them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */

	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];
#if DEBUG_VERBOSE_LEVEL > 1
	#if defined(SAME54) || defined(SAME53)
	vCheckFaultStatusRegisterCortexM4();
	#elif defined(SAMD20)
	prvCheckFaultStatusRegisterCortexM0();
	#endif
#endif
#if DEBUG_VERBOSE_LEVEL > 0
	DEBUG_printf( ("Hard Fault!\n") );
	DEBUG_printf( ("R0: 0x%08lX\nR1: 0x%08lX\nR2: 0x%08lX\n", r0, r1, r2) );
	DEBUG_printf( ("R3: 0x%08lX\nR12: 0x%08lX\nLR: 0x%08lX\n", r3, r12, lr) );
	DEBUG_printf( ("PC: 0x%08lX\nPSR: 0x%08lX\n", pc, psr) );
#else
	UNUSED(r0);
	UNUSED(r1);
	UNUSED(r2);
	UNUSED(r3);
	UNUSED(r12);
	UNUSED(lr);
	UNUSED(pc);
	UNUSED(psr);
#endif
	/* When the following line is hit, the variables contain the register values. */
	#ifdef DEBUG
	for(;;){}
	#else
	utils_WatchDogReset();
	#endif
}

/* The prototype shows it is a naked function - in effect this is just an
assembly function. */
//static void HardFault_Handler( void ) __attribute__( ( naked ) );

/* The fault handler implementation calls a function called
prvGetRegistersFromStack(). */
void HardFault_Handler(void)
{
    asm volatile(
    "movs r0, #4								\t\n"
    "mov  r1, lr								\t\n"
    "tst  r0, r1								\t\n" /* Check EXC_RETURN[2] */
    "beq 1f										\t\n"
    "mrs r0, psp								\t\n"
    "ldr r1,=prvGetRegistersFromStack			\t\n"
    "bx r1										\t\n"
    "1:mrs r0,msp								\t\n"
    "ldr r1,=prvGetRegistersFromStack			\t\n"
	"bx r1										\t\n"
    : /* no output */
    : /* no input */
    : "r0" /* clobber */
    );
}

void MemManagement_Handler(void) {
	
	#if defined(SAME54) || defined(SAME53)
	vPrintMemManagementFaultCortexM4();
	#elif defined(SAMD20)
	
	#endif
	
	for(;;) {}
}

void BusFault_Handler(void) {
	
	DEBUG_printf( ("Bus Fault!\n") );
	
	for(;;) {}
}

void UsageFault_Handler(void) {
	
	#if defined(SAME54) || defined(SAME53)
	vPrintUsageFaultCortexM4();
	#elif defined(SAMD20)
	
	#endif
	
	for(;;) {}
}

void DebugMonitor_Handler(void) {
	for(;;) {}
}