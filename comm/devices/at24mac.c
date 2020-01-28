/*
 * at24mac.c
 *
 * Created: 2019-06-11 1:42:29 PM
 *  Author: Wizz
 */ 

#include "at24mac.h"

#include "i2c.h"

#define AT24MAC_I2C_EEPROM_ADDR 0xA0
#define AT24MAC_I2C_UUID_ADDR 0xB1

BaseType_t at24mac_getMacAddress48(i2cConfig_t* config, uint8_t *macAddress) {
	i2c_master_initIF(config);

	i2c_master_writeAddr_LE(config, AT24MAC_I2C_EEPROM_ADDR, 0x9A);

	uint8_t bytesRead = i2c_master_readBytes_LE(config, AT24MAC_I2C_UUID_ADDR, macAddress, 6);

	return bytesRead == 6;
}

uint16_t at24mac_saveToEEPROM(i2cConfig_t* config, uint8_t addr, uint8_t *buf, uint16_t bufsize) {
	i2c_master_initIF(config);

	return i2c_master_writeBytes_LE(config, AT24MAC_I2C_EEPROM_ADDR, addr, (uint8_t*)buf, bufsize);
}

uint16_t at24mac_loadFromEEPROM(i2cConfig_t* config, uint8_t addr, uint8_t *buf, uint16_t bufsize) {
	i2c_master_initIF(config);

	i2c_master_writeAddr_LE(config, AT24MAC_I2C_EEPROM_ADDR, addr);

	return i2c_master_readBytes_LE(config, AT24MAC_I2C_EEPROM_ADDR, buf, bufsize);
}