/*
 * serialnumber.c
 *
 * Created: 2019-04-04 3:05:57 PM
 *  Author: Wizz
 */ 

#include "serialnumber.h"

#include "sam_ports.h"

// Serial Number
#if defined(SAME53) || defined(SAME54)
	#define SN_WORD0_ADDR	0x008061FC
	#define SN_WORD1_ADDR	0x00806010
#elif defined(SAMD20)
	#define SN_WORD0_ADDR	0x0080A00C
	#define SN_WORD1_ADDR	0x0080A040
#else
	#error Specified device not supported.
#endif

void getSerialNumber(void) {
	uint32_t *ptr = (uint32_t *) SN_WORD0_ADDR;
	g_serialnumber.word0 = *ptr;
	ptr = (uint32_t *) SN_WORD1_ADDR;
	g_serialnumber.word1 = *ptr++;
	g_serialnumber.word2 = *ptr++;
	g_serialnumber.word3 = *ptr;

	g_serialnumber_ptr = (uint32_t*)&g_serialnumber;
}