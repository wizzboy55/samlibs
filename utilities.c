/*
 * utilities.c
 *
 * Created: 2019-05-20 2:05:59 PM
 *  Author: Wizz
 */ 

#include "utilities.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hpl_pm_base.h"

#include "debug_interface.h"

#include "sam_ports.h"

RESETCAUSE_Type utils_rcause;

void utils_SaveResetCause(void) {
	utils_rcause.reg = ResetCause->RCAUSE.reg;
}

void utils_DebugPrintResetCause(void) {
#if DEBUG_VERBOSE_LEVEL > 0
	char *str_cause = 0;
	
	char str_cause_SYST[] = "SysRst";
	char str_cause_WDT[] = "WDT";
	char str_cause_POR[] = "POR";
	char str_cause_EXT[] = "ExtRst";
	char str_cause_BOD12[] = "BO1V2";
	char str_cause_BOD33[] = "BO3V3";

	if(utils_rcause.bit.SYST) {
		str_cause = str_cause_SYST;
	} else if(utils_rcause.reg & ResetCause_WDT) {
		str_cause = str_cause_WDT;
	} else if(utils_rcause.bit.POR) {
		str_cause = str_cause_POR;
	} else if(utils_rcause.bit.EXT) {
		str_cause = str_cause_EXT;
	} else if(utils_rcause.bit.ResetCause_BOD12) {
		str_cause = str_cause_BOD12;
	} else if(utils_rcause.bit.ResetCause_BOD33) {
		str_cause = str_cause_BOD33;
	}

	DEBUG_printf( ("Rst: %s\n", str_cause) );
#endif
}

inline BaseType_t utils_IsSystemReset(void) {
	return utils_rcause.bit.SYST;
}

inline BaseType_t utils_IsExternalResetType(void) {
	return utils_rcause.bit.ResetCause_BOD12 || utils_rcause.bit.ResetCause_BOD33 || utils_rcause.bit.EXT || utils_rcause.bit.POR;
}

inline BaseType_t utils_IsWatchDogTimerReset(void) {
	return utils_rcause.reg & ResetCause_WDT;
}

void watchdog_reset(void) {
	DEBUG_printf( ("BTLDR ERR: WDT Rst\n") );
//	WDT->WDT_CTRLA.bit.ALWAYSON = 1;
	WDT->WDT_CTRLA.bit.ENABLE = 1;
	WDT->INTENCLR.bit.EW = 1;
	WDT->CONFIG.bit.PER = 0x00;
	WDT->CLEAR.reg = 0x00;	// Writing any other value than 0xA5 should initiate a reset
}

void utils_SystemReset(void) {
	NVIC_SystemReset();
}

void utils_WatchDogReset(void) {
	watchdog_reset();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {

	DEBUG_printf( ("StackOverflow! Task: %s\n", pcTaskName) );

	#ifdef DEBUG
	for(;;){}
	#else
	watchdog_reset();
	#endif
}

void vApplicationMallocFailedHook(void) {

	DEBUG_printf( ("Malloc Fail!\n") );

	#ifdef DEBUG
	for(;;){}
	#else
	watchdog_reset();
	#endif
}

void vAssertCalled(const char *pcFileName, unsigned long ulLine) {

	DEBUG_printf( ("ASSERT! F: %s L: %lu\n", pcFileName, ulLine) );

	#ifdef DEBUG
	for(;;){}
	#else
	watchdog_reset();
	#endif
}

void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
#if DEBUG_VERBOSE_LEVEL > 0
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

	DEBUG_printf( ("Hard Fault!\n") );
	DEBUG_printf( ("R0: 0x%08lX\nR1: 0x%08lX\nR2: 0x%08lX\n", r0, r1, r2) );
	DEBUG_printf( ("R3: 0x%08lX\nR12: 0x%08lX\nLR: 0x%08lX\n", r3, r12, lr) );
	DEBUG_printf( ("PC: 0x%08lX\nPSR: 0x%08lX\n", pc, psr) );
#endif
	/* When the following line is hit, the variables contain the register values. */
	#ifdef DEBUG
	for(;;){}
	#else
	watchdog_reset();
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
	for(;;) {}
}

void BusFault_Handler(void) {
	for(;;) {}
}

void UsageFault_Handler(void) {
	for(;;) {}
}

void SVCall_Handler(void) {
	for(;;) {}
}

void DebugMonitor_Handler(void) {
	for(;;) {}
}

uint8_t hexCharToUint(const char c) {
	uint8_t hexValue = 0;
	if((c >= '0') && (c <= '9')) {
		hexValue = (c - '0');
	} else if((c >= 'A') && (c <= 'F')) {
		hexValue = (c - ('A' - 0x0A));
	} else if((c >= 'a') && (c <= 'f')) {
		hexValue = (c - ('a' - 0x0A));
	} else {
		configASSERT(0);
	}

	return hexValue;
}

uint8_t hexConverter(const char *data) {
	return (hexCharToUint(data[0]) << 4) | hexCharToUint(data[1]);
}

uint8_t hexParseUint8(const char **c) {
	uint8_t val = hexConverter(*c);
	(*c) += 2 * sizeof(char);
	return val;
}

uint16_t hexParseUint16(const char **c) {
	uint16_t val = (hexConverter(*c) << 8);
	(*c) += 2 * sizeof(char);
	val |= hexConverter(*c);
	(*c) += 2 * sizeof(char);
	return val;
}

uint32_t hexParseUint32(const char **c) {
	uint32_t val = (hexParseUint16(c) << 16);
	val |= hexParseUint16(c);
	return val;
}

#define RAMFUNC __attribute__ ((section(".ramfunc")))
//#define OPTIMIZE_HIGH __attribute__((optimize(s)))

// Delay loop is put to SRAM so that FWS will not affect delay time
//OPTIMIZE_HIGH
RAMFUNC
void portable_delay_cycles(uint32_t n)
{
	__asm (
	"loop: DMB	\n"
	#ifdef __ICCARM__
	"SUBS r0, r0, #1 \n"
	#else
	"SUB r0, r0, #1 \n"
	#endif
	"CMP r0, #0  \n"
	"BNE loop         "
	);
}