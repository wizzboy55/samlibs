/*
 * i2c.c
 *
 * Created: 2019-05-22 10:36:08 AM
 *  Author: Wizz
 */ 

#include "i2c.h"
#include "samclk.h"
#include "samgpio.h"
#include "task.h"

#define I2C_READMASK 0x01

#define SERCOM_I2CM_CTRLA_MODE_I2CM 0x05

#ifdef SERCOM_I2CM_CTRLA_LOWTOUT
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUT
#elif defined (SERCOM_I2CM_CTRLA_LOWTOUTEN)
#define SERCOM_I2CM_CTRLA_LOWTOUT_Val SERCOM_I2CM_CTRLA_LOWTOUTEN
#endif

uint8_t i2c_mst_writeBytes16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num);
uint8_t i2c_mst_writeAddr16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr);
uint8_t i2c_mst_readBytes_BE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num);

static SemaphoreHandle_t i2cGlobalMutex = NULL;

void i2c_master_initIF(i2cConfig_t* config) {

	i2c_mst_init(config);
	return;
}

/*
// Send device address + read 1 byte
uint8_t i2c_master_readByte(i2cConfig_t* config, uint8_t device) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
	
	sercomdevice->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Acknowledgment Action + Stop

	return sercomdevice->I2CM.DATA.reg;
}
//*/

/*
// Write device address + Read a number of bytes of data, NACK the last byte read
uint8_t i2c_master_readBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device | I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	uint16_t i;

	for(i = 0; i < num; i++) {
		if(i < (num - 1)) {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
			buf[i] = sercomdevice->I2CM.DATA.reg;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x02;	// Send ACK
			} else {
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 1;
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send NACK + Stop
			buf[i] = sercomdevice->I2CM.DATA.reg;
			break;
		}
		uint16_t timeout = 0xFFFF;
		while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1) {
			if(timeout-- == 0) {
				return 0;
			}
		}
	}

	return i;
}
//*/

// Send device address + read multiple bytes, NACK at the end
uint8_t i2c_master_readBytes_BE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num) {

	return i2c_mst_readBytes_BE(config, device, buf, num);
}

/*
uint8_t i2c_master_writeAddr_LE(i2cConfig_t* config, uint8_t device, uint8_t addr) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR);

	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return pdFALSE;
	}

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR);
	
	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return pdFALSE;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}
//*/

/*
uint8_t i2c_master_writeAddr16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);
	
	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}
//*/

uint8_t i2c_master_writeAddr16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr) {
	
	return i2c_mst_writeAddr16_BE(config, device, addr);
}

/*
uint8_t i2c_master_writeByte(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t data) {
	
	Sercom* sercomdevice = (Sercom *)config->sercom;
	
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;
	
	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = data;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}
//*/

/*
uint8_t i2c_master_writeBytes_LE(i2cConfig_t* config, uint8_t device, uint8_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);
	
	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercomdevice->I2CM.DATA.reg = data[bytesSent];

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}
//*/

/*
uint8_t i2c_master_writeBytes16_LE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num) {
	uint16_t bytesSent = 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x01);
	
	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0;

	sercomdevice->I2CM.ADDR.reg = device & ~I2C_READMASK;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = addr & 0x00FF;
	
	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	sercomdevice->I2CM.DATA.reg = (addr >> 8) & 0xFF;

	while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

	while(bytesSent < num) {
		sercomdevice->I2CM.DATA.reg = data[bytesSent];

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);

		if(sercomdevice->I2CM.STATUS.bit.RXNACK) {
			return pdFALSE;	// Slave NACKed
		}

		bytesSent++;
	}

	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03;	// Send Stop

	return pdTRUE;
}
//*/

uint8_t i2c_master_writeBytes16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num) {

	return i2c_mst_writeBytes16_BE(config, device, addr, data, num);
}

/*
	Initialize the I2C interface
*/
void i2c_mst_init(i2cConfig_t* config) {

	if(config->mutex != NULL)
		return;


	// PATCH : We use a global I2C mutex here instead of one mutex per I2C channel
	// because it breaks otherwise, as if writing to some register in one SERCOM would
	// affect the other. Not sure why. Having one global mutex for any i2c communication.
	// TODO : Find why this is and correct the source of the problem, we should be able
	// to use one mutex per channel.
	if(i2cGlobalMutex == NULL)
		i2cGlobalMutex = xSemaphoreCreateMutex();
	config->mutex = i2cGlobalMutex; // Associate channel mutex to global channel
	

	// config->mutex = xSemaphoreCreateMutex(); // Create one mutex per I2C channel

	Sercom* sercomdevice = (Sercom *)config->sercom;

	// if(sercomdevice->I2CM.CTRLA.bit.MODE == SERCOM_I2CM_CTRLA_MODE_I2CM) {
	// 	return;
	// }

	samclk_enable_peripheral_clock(sercomdevice);
	samclk_enable_gclk_channel(sercomdevice, config->clksource);
	samclk_enable_glck_slow_channel(sercomdevice, 11);

	sercomdevice->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SWRST;

	// Wait while reset is in effect
	while(sercomdevice->I2CM.CTRLA.bit.SWRST) {}

	sercomdevice->I2CM.CTRLA.bit.MODE = SERCOM_I2CM_CTRLA_MODE_I2CM;
	
	sercomdevice->I2CM.CTRLA.bit.LOWTOUTEN = 1;
	
	sercomdevice->I2CM.CTRLA.bit.INACTOUT = 1;

	// We are assuming GCLK source clock is the main clock (CONF_CPU_FREQUENCY) divided by DIV to calculate the baud
	uint8_t div = GCLK->GENCTRL[config->clksource].bit.DIV == 0 ? 1 : GCLK->GENCTRL[config->clksource].bit.DIV; // Ensure != 0
	uint16_t baud = CONF_CPU_FREQUENCY / div / (2 * config->baudrate) - 1;
	ASSERT(baud <= 0xff); // Invalid baudrate for the chosen clock source
	sercomdevice->I2CM.BAUD.reg = baud;

	sercomdevice->I2CM.INTENCLR.reg = 0xFF;

	sercomdevice->I2CM.CTRLA.bit.ENABLE = 1;

	sercomdevice->I2CM.STATUS.bit.BUSSTATE = 0x01;
	while(sercomdevice->I2CM.SYNCBUSY.bit.SYSOP == 1);

	samgpio_setPinFunction(config->pin_sda, config->pinmux);
	samgpio_setPinFunction(config->pin_scl, config->pinmux);
}

/*
	Used by other i2c functions to start a transaction.

	- Sends a start or repeat start
	- Sends the first control byte (7 bit address + read bit)
*/

static bool i2c_mst_start(i2cConfig_t* config, uint8_t slave_addr, bool readFlag)
{
	if(xTaskGetCurrentTaskHandle() != xSemaphoreGetMutexHolder(config->mutex)) // Current task does not already own the mutex
	{
		BaseType_t ret = xSemaphoreTake(config->mutex, portMAX_DELAY);
		// debug_printf("Mutex %x, %x\n", config, config->mutex);
	}

	Sercom* sercomdevice = (Sercom *)config->sercom;

	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE == 0x03); // I2C Bus Busy

	uint8_t initial_bus_state = sercomdevice->I2CM.STATUS.bit.BUSSTATE;

	uint8_t addr = readFlag ? (slave_addr << 1) | I2C_READMASK : slave_addr << 1;

	sercomdevice->I2CM.ADDR.reg = addr; // Will trigger a start or repeat start depending on bus state (IDLE vs OWNED)

	// debug_printf("Bus state %x, %x, %d\n", config, slave_addr, sercomdevice->I2CM.STATUS.bit.BUSSTATE);
	// debug_printf("Initial state %x, %x, %d\n", config, slave_addr, initial_bus_state);
	if(initial_bus_state == 0x01 || initial_bus_state == 0x00)
		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1);
	else
	{
		while(sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1)
		{
			if(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x02)
			{
				// It gets stuck here at one point if we use one mutex per port instead of a global mutex
				// debug_printf("Waiting on %x, %x\n", config, slave_addr);
			}
		}
	}

	return sercomdevice->I2CM.STATUS.bit.RXNACK == 0;
}

void i2c_mst_stop(i2cConfig_t* config)
{
	Sercom* sercomdevice = (Sercom *)config->sercom;
	sercomdevice->I2CM.CTRLB.bit.CMD = 0x03; // ACK/NACK + stop
	while(sercomdevice->I2CM.STATUS.bit.BUSSTATE == 0x02);
	xSemaphoreGive(config->mutex);
	// debug_printf("stop %x\n", config);
}


/*
	Write multiple bytes to the I2C bus

	- Sends a start condition if the bus is IDLE or repeat start if the bus is OWNED by
	- Writes the slave_addr and read/write flag
	- Writes the buffer and validate the ACK for each byte
	- Does not end the transaction automatically (no stop bit sent)

	Returns the number of bytes written
*/
uint16_t i2c_mst_write(i2cConfig_t* config, uint8_t slave_addr, uint8_t *buf, uint8_t buf_size)
{
	if(!i2c_mst_start(config, slave_addr, false))
		return 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	if(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x02) // We don't own the I2C bus
		return 0;

	for(uint8_t i = 0; i < buf_size; i++)
	{
		sercomdevice->I2CM.DATA.reg = buf[i]; // Write data

		while(sercomdevice->I2CM.INTFLAG.bit.MB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1) // Wait for operation to finish
		{

		}

		if(sercomdevice->I2CM.STATUS.bit.RXNACK == 1)
			return i;
	}

	return buf_size;
}

/*
	Read multiple bytes on the I2C bus

	- Sends a start condition if the bus is IDLE or repeat start if the bus is OWNED by
	- Writes the slave_addr and read/write flag
	- Writes the buffer and validate the ACK for each byte and NACK for the last byte
	- Does not end the transaction automatically (no stop bit sent)

	Returns the number of bytes read
*/
uint16_t i2c_mst_read(i2cConfig_t* config, uint8_t slave_addr, uint8_t *buf, uint8_t buf_size)
{
	if(!i2c_mst_start(config, slave_addr, true))
		return 0;	


	Sercom* sercomdevice = (Sercom *)config->sercom;

	uint16_t i;
	for(i = 0; i < buf_size; i++)
	{
		// Wait for a byte or an error
		while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR) {
			if(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x02) // Something's wrong, we should be in control of the bus
				break;
		}
		
		if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
			// sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
			// i2c_mst_stop(config);
			// debug_printf("I2C Error mst_read\n");
			break;
		}

		if(i == buf_size - 1) // Last byte to read
		{
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 1; // Set to NACK
			// We don't really send the NACK here, it will be sent with the next start or stop action
		}
		else
		{
			sercomdevice->I2CM.CTRLB.bit.ACKACT = 0; // Set to ACK
			sercomdevice->I2CM.CTRLB.bit.CMD = 0x02; // Send the ACK
		}

		buf[i] = sercomdevice->I2CM.DATA.reg; // Read byte
	}

	return i;
}

/*
	Reads a single byte

	- Sends a start or repeated start condition
	- Writes the slave address and read flag
	- Reads a single byte and sends ACK
	- Does not stop the transaction (no STOP signal)

	Returns the value of the read byte or 0
*/
uint8_t i2c_mst_readByte(i2cConfig_t* config, uint8_t slave_addr)
{
	if(!i2c_mst_start(config, slave_addr, true))
		return 0;

	Sercom* sercomdevice = (Sercom *)config->sercom;

	// Wait for a byte or an error
	while(sercomdevice->I2CM.INTFLAG.bit.SB != 1 && sercomdevice->I2CM.STATUS.bit.CLKHOLD != 1 && !sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		if(sercomdevice->I2CM.STATUS.bit.BUSSTATE != 0x02) // Something's wrong, we should be in control of the bus
			return 0;
	}
	
	if(sercomdevice->I2CM.INTFLAG.bit.ERROR) {
		sercomdevice->I2CM.INTFLAG.bit.ERROR = 1; // Clear error flag
		return 0;
	}

	uint8_t byte = sercomdevice->I2CM.DATA.reg; // Read byte

	sercomdevice->I2CM.CTRLB.bit.ACKACT = 0; // ACK
	// sercomdevice->I2CM.CTRLB.bit.CMD = 0x03; // ACK + STOP

	return byte;
}



//
//	Higher layer functions
//

// Don't use this function, kept for compatibility only
uint8_t i2c_mst_writeBytes16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr, uint8_t *data, uint16_t num) {

	// Write slave address (8 bits) + register address (16 bits, big) + register value (16 bits, big)
	uint8_t buf[6];
	buf[0] = (addr >> 8) & 0xFF;
	buf[1] = addr & 0xFF;

	for(uint8_t i = 0; i < num; i ++)
		buf[i+2] = data[num-i-1];

	i2c_mst_write(config, device>>1, buf, 2+num);
	i2c_mst_stop(config);

	return pdTRUE;
}


uint8_t i2c_mst_writeAddr16_BE(i2cConfig_t* config, uint8_t device, uint16_t addr) {
	// Write register address
	uint8_t buf[2];
	buf[0] = (addr >> 8) & 0xFF;
	buf[1] = addr & 0xFF;
	i2c_mst_write(config, device >> 1, buf, 2);
	i2c_mst_stop(config);
	return pdTRUE;
}


// Send device address + read multiple bytes, NACK at the end
uint8_t i2c_mst_readBytes_BE(i2cConfig_t* config, uint8_t device, uint8_t *buf, uint16_t num) {
	
	ASSERT(num <= 4);
	
	uint8_t tmp[4];
	uint8_t i = i2c_mst_read(config, device >> 1, tmp, num);
	i2c_mst_stop(config);

	for(uint8_t i = 0; i < num; i++)
		buf[i] = tmp[num-i-1]; // To big Endian

	return i;
}
