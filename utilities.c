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

void utils_WatchDogReset(void) {
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