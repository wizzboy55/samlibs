/*
 * at24mac.c
 *
 * Created: 2019-06-11 1:42:29 PM
 *  Author: Wizz
 */ 

#include "at24mac.h"

#include "i2c.h"

#define AT24MAC_SERCOM SERCOM4
#define AT24MAC_SERCOM_CLK SERCOM4_GCLK_ID_CORE
#define AT24MAC_I2C_BAUDRATE 400000
#define AT24MAC_I2C_EEPROM_ADDR 0xA0
#define AT24MAC_I2C_UUID_ADDR 0xB1

void at24mac_initI2C(void) {
	// TODO Add a check for SERCOM mode to prevent useless inits
	
	i2c_master_initIF(AT24MAC_SERCOM, AT24MAC_SERCOM_CLK, AT24MAC_I2C_BAUDRATE, GPIO(GPIO_PORTB, 8), GPIO(GPIO_PORTB, 9), 3);
}

BaseType_t at24mac_getMacAddress48(uint8_t *macAddress) {
	at24mac_initI2C();

	i2c_master_writeAddr(AT24MAC_SERCOM, AT24MAC_I2C_EEPROM_ADDR, 0x9A);

	uint8_t bytesRead = i2c_master_readBytes(AT24MAC_SERCOM, AT24MAC_I2C_UUID_ADDR, macAddress, 6);

	return bytesRead == 6;
}

uint16_t at24mac_saveToEEPROM(uint8_t addr, uint8_t *buf, uint16_t bufsize) {
	at24mac_initI2C();

	return i2c_master_writeBytes(AT24MAC_SERCOM, AT24MAC_I2C_EEPROM_ADDR, addr, (uint8_t*)buf, bufsize);
}

uint16_t at24mac_loadFromEEPROM(uint8_t addr, uint8_t *buf, uint16_t bufsize) {
	at24mac_initI2C();

	i2c_master_writeAddr(AT24MAC_SERCOM, AT24MAC_I2C_EEPROM_ADDR, addr);

	return i2c_master_readBytes(AT24MAC_SERCOM, AT24MAC_I2C_EEPROM_ADDR, buf, bufsize);
}