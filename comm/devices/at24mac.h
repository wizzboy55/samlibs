/*
 * at24mac.h
 *
 * Created: 2019-06-11 1:42:36 PM
 *  Author: Wizz
 */ 


#ifndef AT24MAC_H_
#define AT24MAC_H_

#include <stdint.h>
#include "FreeRTOS.h"

void at24mac_initI2C(void);
BaseType_t at24mac_getMacAddress48(uint8_t *macAddress);
uint16_t at24mac_saveToEEPROM(uint8_t addr, uint8_t *buf, uint16_t bufsize);
uint16_t at24mac_loadFromEEPROM(uint8_t addr, uint8_t *buf, uint16_t bufsize);

#endif /* AT24MAC_H_ */