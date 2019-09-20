/**
  ENC28J60 Ethernet Driver

  Company:
    Charles Gervais-Dumont

  File Name:
    ENC28J60_driver.c

  Summary:
    This is the Ethernet driver implementation for ENC28J60 family devices.

  Description:
    This file provides the Ethernet driver API implementation for
    the ENC28J60 device.

 */

#include <stdint.h>
#include <stddef.h>
#include <hpl_pm_base.h>
#include <hpl_gclk_base.h>
#include <hal_gpio.h>
#include <hal_io.h>
#include <peripheral_clk_config.h>
#include <hal_ext_irq.h>

#include "enc28j60.h"
#include "enc28j60_registers.h"

#define ENC28J60_SERCOM SERCOM0

#define ENC28J60_CS_pin	10
#define ENC28J60_CS_port GPIO_PORTA

#define ENC28J60_NCS_LOW() PORT_IOBUS->Group[ENC28J60_CS_port].OUTCLR.reg = 1 << ENC28J60_CS_pin;
#define ENC28J60_NCS_HIGH() PORT_IOBUS->Group[ENC28J60_CS_port].OUTSET.reg = 1 << ENC28J60_CS_pin;

#define ENC28J60_IRQ_PIN GPIO(GPIO_PORTA, 14)

#define BAUDRATE(x) (CONF_CPU_FREQUENCY / (2 * x)) - 1

#define ENC28J60_PIN_MOSI	GPIO(GPIO_PORTA, 8)
#define ENC28J60_PIN_SCK	GPIO(GPIO_PORTA, 9)
#define ENC28J60_PIN_MISO	GPIO(GPIO_PORTA, 11)

sfr_bank_t lastBank;

#define MAX_TX_PACKET (1500)

#define NOP() do{__asm__ __volatile__ ("nop");} while(0)

/*******************************************************************************/

void ENC_IfInit(void) {

	_pm_enable_bus_clock(PM_BUS_APBC, ENC28J60_SERCOM);
	_gclk_enable_channel(SERCOM0_GCLK_ID_CORE, GCLK_CLKCTRL_GEN_GCLK0_Val);

	ENC28J60_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_DIPO(0x03) | SERCOM_SPI_CTRLA_DOPO(0x00) | SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
	ENC28J60_SERCOM->SPI.BAUD.reg = BAUDRATE(10000000);
	ENC28J60_SERCOM->SPI.CTRLB.reg = SERCOM_SPI_CTRLB_RXEN;

	gpio_set_pin_level(ENC28J60_PIN_MOSI, false);
	gpio_set_pin_direction(ENC28J60_PIN_MOSI, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ENC28J60_PIN_MOSI, PINMUX_PA08C_SERCOM0_PAD0);

	gpio_set_pin_level(ENC28J60_PIN_SCK, false);
	gpio_set_pin_direction(ENC28J60_PIN_SCK, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ENC28J60_PIN_SCK, PINMUX_PA09C_SERCOM0_PAD1);

	gpio_set_pin_direction(ENC28J60_PIN_MISO, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(ENC28J60_PIN_MISO, GPIO_PULL_OFF);
	gpio_set_pin_function(ENC28J60_PIN_MISO, PINMUX_PA11C_SERCOM0_PAD3);

	gpio_set_pin_level(GPIO(ENC28J60_CS_port, ENC28J60_CS_pin), true);
	gpio_set_pin_direction(GPIO(ENC28J60_CS_port, ENC28J60_CS_pin), GPIO_DIRECTION_OUT);

	ENC28J60_SERCOM->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;	
}

static inline void ENC28J60_SERCOM_write(uint8_t *buf, uint8_t len) {
	// Transmit
	while(len--) {
		while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
			// Wait for register to empty
		}
		ENC28J60_SERCOM->SPI.DATA.reg = *buf++;
	}

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)) {
		// Wait for end of transmission
	}
}

static inline uint8_t ENC28J60_SERCOM_writeread(uint8_t w) {
	
	uint8_t data;

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect dummy byte

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect dummy byte

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
		// Wait for register to empty
	}
	ENC28J60_SERCOM->SPI.DATA.reg = w;

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
		// Wait for data register empty
	}
	
	ENC28J60_SERCOM->SPI.DATA.reg = 0xFF;	// Transmit empty byte

	while(!((ENC28J60_SERCOM->SPI.INTFLAG.reg & (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_TXC)) == (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_TXC))) {
		// Wait for end of transmission
	}

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect dummy byte

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect actual data

	return data;
}

static inline uint8_t ENC28J60_SERCOM_writeread_macmii(uint8_t w) {
	
	uint8_t data;

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect dummy byte

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg; // Collect dummy byte

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
		// Wait for register to empty
	}
	ENC28J60_SERCOM->SPI.DATA.reg = w;

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
		// Wait for data register empty
	}
	
	ENC28J60_SERCOM->SPI.DATA.reg = 0xFF;	// Transmit empty byte

	while(!((ENC28J60_SERCOM->SPI.INTFLAG.reg & (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_DRE)) == (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_DRE))) {
		// Wait for end of transmission
	}

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	ENC28J60_SERCOM->SPI.DATA.reg = 0xFF;	// Transmit empty byte

	while(!((ENC28J60_SERCOM->SPI.INTFLAG.reg & (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_TXC | SERCOM_SPI_INTFLAG_DRE)) == (SERCOM_SPI_INTFLAG_RXC | SERCOM_SPI_INTFLAG_TXC | SERCOM_SPI_INTFLAG_DRE))) {
		// Wait for end of transmission
	}

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	return data;
}

static inline void ENC28J60_SERCOM_readBuffer(uint8_t *buf, uint16_t len) {

	uint8_t data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;
	
	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {
		// Wait for register to empty
	}
	ENC28J60_SERCOM->SPI.DATA.reg = rbm_inst;

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)) {
		// Wait for end of transmission
	}
	
	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;
	data = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	while(len--) {

		ENC28J60_SERCOM->SPI.DATA.reg = 0xFF;	// Transmit empty byte

		while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)) {
			// Wait for end of transmission
		}

		*buf++ = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;
	}

	while(!(ENC28J60_SERCOM->SPI.INTFLAG.reg & SERCOM_SPI_INTFLAG_TXC)) {
		// Wait for end of transmission
	}

	*buf++ = (uint8_t)ENC28J60_SERCOM->SPI.DATA.reg;

	data++;		// Remove warning for dump variable
}

/**
 * Bank Select
 * @param r
 */
static inline void ENC_BankSel(enc28j60_registers_t r)
{
	uint8_t a = r & BANK_MASK;

	if (a != sfr_common && a != lastBank) {
		lastBank = a;
		uint8_t buf[2] = {bfc_inst | (J60_ECON1 & SFR_MASK), 0x03};	// clear the bank bits
		ENC28J60_NCS_LOW();
		ENC28J60_SERCOM_write(buf, sizeof(buf));
		ENC28J60_NCS_HIGH();
		uint8_t buf2[2] = {bfs_inst | (J60_ECON1 & SFR_MASK), a >> 6}; // set the needed bits
		ENC28J60_NCS_LOW();
		ENC28J60_SERCOM_write(buf2, sizeof(buf2));
		ENC28J60_NCS_HIGH();
	}
}

/**
 * Read 1 byte from SFRs
 * @param a
 * @return
 */
uint8_t ENC_ReadCtrl8(enc28j60_registers_t a)
{
	uint8_t v;
	uint8_t buf = rcr_inst | (a & SFR_MASK);

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	v = ENC28J60_SERCOM_writeread(buf);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE();

	return v;
}

uint8_t ENC_ReadCtrl8_MacMii(enc28j60_registers_t a) {
	uint8_t v;
	uint8_t buf = rcr_inst | (a & SFR_MASK);

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	v = ENC28J60_SERCOM_writeread_macmii(buf);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()

	return v;
}

/**
 * Read 2 bytes from SFRs
 * @param a
 * @return
 */
uint16_t ENC_ReadCtrl16(enc28j60_registers_t a)
{
	uint8_t b1, b2;
	uint8_t buf = rcr_inst | (a);

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	a &= SFR_MASK;
	ENC28J60_NCS_LOW();
	b1 = ENC28J60_SERCOM_writeread(buf);
	ENC28J60_NCS_HIGH();
	NOP();
	NOP();
	ENC28J60_NCS_LOW();
	buf = rcr_inst | (a + 1);
	b2 = ENC28J60_SERCOM_writeread(buf);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()

	uint16_t v = ((uint16_t)b2 << 8 | b1);

	return v;
}

uint16_t ENC_ReadCtrl16_MacMii(enc28j60_registers_t a) {
	uint8_t b1, b2;
	uint8_t buf = rcr_inst | (a);

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	a &= SFR_MASK;
	ENC28J60_NCS_LOW();
	b1 = ENC28J60_SERCOM_writeread_macmii(buf);
	ENC28J60_NCS_HIGH();
	NOP();
	NOP();
	ENC28J60_NCS_LOW();
	buf = rcr_inst | (a + 1);
	b2 = ENC28J60_SERCOM_writeread_macmii(buf);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()

	uint16_t v = ((uint16_t)b2 << 8 | b1);

	return v;
}

/**
 * Write 1 byte to ENC
 * @param v
 */
void ENC_Write8(uint8_t v)
{
	CRITICAL_SECTION_ENTER()

	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(&v, 1);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()
}

/**
 * Write 1 byte to SFRs
 * @param a
 * @param v
 */
void ENC_WriteCtrl8(enc28j60_registers_t a, uint8_t v)
{
	uint8_t buf[2] = {(wcr_inst | (a & SFR_MASK)), v};

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(buf, 2);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()
}

/**
 * Write 2 bytes to SFRs
 * @param a
 * @param v
 */
void ENC_WriteCtrl16(enc28j60_registers_t a, uint16_t v)
{
	ENC_WriteCtrl8(a, ((char *)&v)[0]);
	ENC_WriteCtrl8(a+1, ((char *)&v)[1]);
}

/**
 * Write Operation
 * @param op
 * @param a
 * @param d
 */
void ENC_WriteOp(spi_inst_t op, enc28j60_registers_t a, uint8_t d)
{
	uint8_t buf[2] = {(op | (a & SFR_MASK)), d};

	CRITICAL_SECTION_ENTER()

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(buf, 2);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()
}

/**
 * Write PHY  register
 * @param a
 * @param d
 */
void ENC_WritePhy(enc28j60_phy_registers_t a, uint16_t d)
{
	uint8_t v = 1;

	ENC_WriteCtrl8(J60_MIREGADR, a);
	ENC_WriteCtrl16(J60_MIWRL, d);
	while (v & 0x01) {
		v = ENC_ReadCtrl8_MacMii(J60_MISTAT);
	}
}

/**
 * Read PHY register
 * @param a
 * @return
 */
uint16_t ENC_ReadPhy(enc28j60_phy_registers_t a)
{
	ENC_WriteCtrl8(J60_MIREGADR, a);
	ENC_WriteOp(bfs_inst, J60_MICMD, MICMD_MIIRD); // set the read flag
	while (ENC_ReadCtrl8_MacMii(J60_MISTAT) & MISTAT_BUSY);	// wait for the busy flag to clear
	ENC_WriteOp(bfc_inst, J60_MICMD, MICMD_MIIRD); // clear the read flag

	return ENC_ReadCtrl16_MacMii(J60_MIRDL);
}

/**
 * Read block of data from the RX Buffer
 * @param buffer
 * @param length
 * @return
 */
void ENC_ReadBuffer(uint8_t *buffer, uint16_t length)
{
	CRITICAL_SECTION_ENTER()

	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_readBuffer(buffer, length);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()
}

/**
 * Write a block of data to TX Buffer
 * @param data
 * @param length
 */
void ENC_WriteBuffer(uint8_t *data, uint16_t length)
{
	uint8_t buf = wbm_inst;

	CRITICAL_SECTION_ENTER()

	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(&buf, 1);
	ENC28J60_SERCOM_write(data, length);
	ENC28J60_NCS_HIGH();

	CRITICAL_SECTION_LEAVE()
}

void ENC_RegisterIrq(void* irq) {
	ext_irq_register(ENC28J60_IRQ_PIN, irq);
}