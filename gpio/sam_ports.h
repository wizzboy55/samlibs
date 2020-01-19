/*
 * sam_ports.h
 *
 * Created: 2019-07-28 6:36:22 PM
 *  Author: Wizz
 */ 


#ifndef SAM_PORTS_H_
#define SAM_PORTS_H_

// Reset Cause
#if defined(SAME54) || defined(SAME53)
	#define RESETCAUSE_Type		RSTC_RCAUSE_Type
	#define ResetCause			RSTC
	#define ResetCause_WDT		RSTC_RCAUSE_WDT
	#define ResetCause_BOD12	BODCORE
	#define ResetCause_BOD33	BODVDD
#elif defined(SAMD20)
	#define RESETCAUSE_Type PM_RCAUSE_Type
	#define ResetCause PM
	#define ResetCause_WDT		PM_RCAUSE_WDT
	#define ResetCause_BOD12	BOD12
	#define ResetCause_BOD33	BOD33
#elif defined(SAMC20)
	#define RESETCAUSE_Type		RSTC_RCAUSE_Type
	#define ResetCause			RSTC
	#define ResetCause_WDT		RSTC_RCAUSE_WDT
	#define ResetCause_BOD12	BODCORE
	#define ResetCause_BOD33	BODVDD
#else
	#error Specified device not supported.
#endif

// WatchDog
#if defined(SAME54) || defined(SAME53)
	#define WDT_CTRLA			CTRLA
#elif defined(SAMD20)
	#define WDT_CTRLA			CTRL
#elif defined(SAMC20)
	#define WDT_CTRLA			CTRLA
#else
	#error Specified device not supported.
#endif

#endif /* SAM_PORTS_H_ */