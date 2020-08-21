
#include "watchdog.h"


void wdog_init(Watchdog_Config_t* config)
{
	samclk_enable_peripheral_clock(WDT); // Enable WDT clock

	if(WDT->WDT_CTRLA.bit.ALWAYSON) // Always ON mode already active
	{
		// Those are the only things we can reconfigure
		WDT->WDT_CTRLA.bit.WEN = config->windowMode;

		if(config->interruptEnable)
			WDT->INTENSET.bit.EW = 1; // Set interrupt enable bit
		else
			WDT->INTENCLR.bit.EW = 1; // Clear interrupt enable bit

		return;
	}

	WDT->WDT_CTRLA.bit.ENABLE = 0;	// Disable watchdog
	while(WDT->SYNCBUSY.bit.ENABLE); // Wait for sync

	WDT->WDT_CTRLA.bit.WEN = config->windowMode;
	WDT->CONFIG.bit.PER = config->period;
	WDT->CONFIG.bit.WINDOW = config->windowPeriod;

	if(config->interruptEnable)
		WDT->INTENSET.bit.EW = 1; // Set interrupt enable bit
	else
		WDT->INTENCLR.bit.EW = 1; // Clear interrupt enable bit

	while(WDT->SYNCBUSY.reg != 0); // Wait for sync of all registers

	if(config->alwaysOnMode)
		WDT->WDT_CTRLA.bit.ALWAYSON = config->alwaysOnMode;
	else
		WDT->WDT_CTRLA.bit.ENABLE = config->enable;
}

void wdog_feed()
{
	WDT->CLEAR.reg = 0xA5; // Reset watchdog timer
}


void wdog_reset_mcu()
{
	//DEBUG_printf( ("Forced wdog reset\n") );
	WDT->WDT_CTRLA.bit.ENABLE = 1;
	WDT->INTENCLR.bit.EW = 1;
	WDT->CONFIG.bit.PER = 0x00;
	WDT->CLEAR.reg = 0x00;	// Writing any other value than 0xA5 should initiate a reset
}
