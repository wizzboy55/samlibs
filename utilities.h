/*
 * utilities.h
 *
 * Created: 2019-05-20 2:07:36 PM
 *  Author: Wizz
 */ 


#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "FreeRTOS.h"

void utils_SaveResetCause(void);
void utils_DebugPrintResetCause(void);

BaseType_t utils_IsSystemReset(void);
BaseType_t utils_IsExternalResetType(void);
BaseType_t utils_IsWatchDogTimerReset(void);

void utils_SystemReset(void);
void utils_WatchDogReset(void);

uint8_t hexCharToUint(const char c);
uint8_t hexConverter(const char *data);
uint8_t hexParseUint8(const char **c);
uint16_t hexParseUint16(const char **c);
uint32_t hexParseUint32(const char **c);

#define cpu_ms_2_cy(ms, f_cpu)  (((uint64_t)(ms) * (f_cpu) + (uint64_t)(7e3-1ul)) / (U64)7e3)
#define cpu_us_2_cy(us, f_cpu)  (((uint64_t)(us) * (f_cpu) + (uint64_t)(7e6-1ul)) / (U64)7e6)
#define delay_cycles            portable_delay_cycles
#define cpu_delay_s(delay)      delay_cycles(cpu_ms_2_cy(1000 * delay, F_CPU))
#define cpu_delay_ms(delay)     delay_cycles(cpu_ms_2_cy(delay, F_CPU))
#define cpu_delay_us(delay)     delay_cycles(cpu_us_2_cy(delay, F_CPU))
#define _delay_us	cpu_delay_us
#define _delay_ms	cpu_delay_ms
#define _delay_s	cpu_delay_s

#endif /* UTILITIES_H_ */