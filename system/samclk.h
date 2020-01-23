/*
 * samclk.h
 *
 * Created: 2019-08-08 9:26:47 AM
 *  Author: Wizz
 */ 


#ifndef SAMCLK_H_
#define SAMCLK_H_

#include <stdint.h>

void samclk_enable_peripheral_clock(const void *const module);
void samclk_enable_gclk_channel(const void *const module, const uint8_t source);
void samclk_enable_glck_slow_channel(const void *const module, const uint8_t slow_source);

#endif /* SAMCLK_H_ */