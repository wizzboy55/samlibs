/*
	SAM Watchdog
*/

#ifndef SAM_WATCHDOG_H
#define SAM_WATCHDOG_H

#include <hpl_pm_base.h>

#include "sam_ports.h"
#include "samclk.h"

typedef enum {
	PERIOD_8_MS		= 0x0,	// 7.8125 ms (CYC8)
	PERIOD_16_MS	= 0x1,	// 15.6 ms (CYC16)
	PERIOD_31_MS	= 0x2,	// 31.25 ms (CYC32)
	PERIOD_62_MS	= 0x3,	// 62.5 ms (CYC64)
	PERIOD_125_MS	= 0x4,	// 125 ms (CYC128)
	PERIOD_250_MS	= 0x5,	// 250 ms (CYC256)
	PERIOD_500_MS	= 0x6,	// 500 ms (CYC512)
	PERIOD_1_SEC	= 0x7,	// 1 second (CYC1024)
	PERIOD_2_SEC	= 0x8,	// 2 seconds (CYC2048)
	PERIOD_4_SEC	= 0x9,	// 4 seconds	 (CYC4096)
	PERIOD_8_SEC	= 0xA,	// 8 seconds (CYC8192)
	PERIOD_16_SEC	= 0xB	// 16 seconds (CYC16384)
} Watchdog_Period_t;

typedef struct {
	bool enable;					// Enable peripheral
	Watchdog_Period_t period;
	bool windowMode;				// Window mode
	Watchdog_Period_t windowPeriod;
	bool interruptEnable;			// Enable Early Warning interrupt
	bool alwaysOnMode;				// Always ON mode
} Watchdog_Config_t;

void wdog_init(Watchdog_Config_t* config);
void wdog_feed();
void wdog_reset_mcu();

#endif