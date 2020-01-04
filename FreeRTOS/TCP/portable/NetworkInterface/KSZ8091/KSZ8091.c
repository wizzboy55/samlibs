/*
 * KSZ8091.c
 *
 * Created: 2019-07-29 2:04:40 PM
 *  Author: Wizz
 */ 

#include "KSZ8091.h"

#include "FreeRTOS.h"

// TODO:
//- Implement Power saving modes + EEE
//- Digital Loopback Test
//- LinkMD Cable Diagnostics
//-= NAND Tree IO Test

BaseType_t xKSZ8091InitializePHY(uint8_t address) {

	return pdPASS;
}