/*
 * serialnumber.h
 *
 * Created: 2019-01-30 11:20:04 AM
 *  Author: Wizz
 */ 


#ifndef SERIALNUMBER_H_
#define SERIALNUMBER_H_

#include <stdint.h>

#define UUID_BIT_LENGTH 128
#define UUID_BYTE_LENGTH UUID_BIT_LENGTH / 8

typedef struct {
	uint32_t word3;
	uint32_t word2;
	uint32_t word1;
	uint32_t word0;
} serialnumber_t;

serialnumber_t g_serialnumber;
uint32_t *g_serialnumber_ptr;

void getSerialNumber(void);

#endif /* SERIALNUMBER_H_ */