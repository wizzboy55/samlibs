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

#include "FreeRTOS.h"
#include "semphr.h"

#include "enc28j60_int.h"
#include "enc28j60_registers.h"
#include "enc28j60_config.h"

sfr_bank_t lastBank;

#define MAX_TX_PACKET (1500)

#define NOP() do{__asm__ __volatile__ ("nop");} while(0)

SemaphoreHandle_t spi_semaphore = NULL;

enum enc28j60_spi_operation {None = 0, Write, WriteRead, WriteRead_MacMii, ReadBuffer};

typedef struct {
	enum enc28j60_spi_operation operation;
	uint8_t *write_buffer;
	uint8_t *read_buffer;
	uint8_t read_data;
	uint16_t write_length;
	uint16_t read_length;
	uint8_t macmii_data[2];
} enc28j60_spi_context_t;

enc28j60_spi_context_t spi_context;

void (*irq_callback)(void) = NULL;

/*******************************************************************************/

void ENC_InitIRQ(void) {
	_gclk_enable_channel(EIC_GCLK_ID, CONF_GCLK_EIC_SRC);

	gpio_set_pin_direction(ENC28J60_PIN_IRQ, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(ENC28J60_PIN_IRQ, GPIO_PULL_UP);
	gpio_set_pin_function(ENC28J60_PIN_IRQ, ENC28J60_FUNC_IRQ);

	ENC28J60_IRQ_EIC_CONFIGSENSE = EIC_CONFIG_SENSE0_FALL_Val;
	ENC28J60_IRQ_EIC_INTENSET = 1;
	EIC->CTRL.bit.ENABLE = 1;
	NVIC_EnableIRQ(EIC_IRQn);
}

void ENC_IfInit(void) {
	_pm_enable_bus_clock(PM_BUS_APBC, ENC28J60_SERCOM);
	_gclk_enable_channel(ENC28J60_SERCOM_CLK, GCLK_CLKCTRL_GEN_GCLK0_Val);

	*ENC28J60_SPI_CTRLA = SERCOM_SPI_CTRLA_DIPO(ENC28J60_DIPO) | SERCOM_SPI_CTRLA_DOPO(ENC28J60_DOPO) | SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
	ENC28J60_SERCOM->SPI.BAUD.reg = BAUDRATE(ENC28J60_SPI_BAUD);
	*ENC28J60_SPI_CTRLB = 0;
	*ENC28J60_SPI_INTENSET = 0;

	spi_semaphore = xSemaphoreCreateBinary();

	gpio_set_pin_level(ENC28J60_PIN_MOSI, false);
	gpio_set_pin_direction(ENC28J60_PIN_MOSI, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ENC28J60_PIN_MOSI, ENC28J60_FUNC_MOSI);

	gpio_set_pin_level(ENC28J60_PIN_SCK, false);
	gpio_set_pin_direction(ENC28J60_PIN_SCK, GPIO_DIRECTION_OUT);
	gpio_set_pin_function(ENC28J60_PIN_SCK, ENC28J60_FUNC_SCK);

	gpio_set_pin_direction(ENC28J60_PIN_MISO, GPIO_DIRECTION_IN);
	gpio_set_pin_pull_mode(ENC28J60_PIN_MISO, GPIO_PULL_OFF);
	gpio_set_pin_function(ENC28J60_PIN_MISO, ENC28J60_FUNC_MISO);

	gpio_set_pin_level(GPIO(ENC28J60_CS_port, ENC28J60_CS_pin), true);
	gpio_set_pin_direction(GPIO(ENC28J60_CS_port, ENC28J60_CS_pin), GPIO_DIRECTION_OUT);

	ENC28J60_SERCOM->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
	NVIC_EnableIRQ(ENC28J60_SERCOM_IRQ);
}

static inline void ENC28J60_SERCOM_write(uint8_t *buf, uint16_t len) {
	if(len) {
		spi_context.operation = Write;
		*ENC28J60_SPI_DATA = *buf;
		spi_context.write_buffer = ++buf;
		spi_context.write_length = len - 1;
		*ENC28J60_SPI_INTENSET = SERCOM_SPI_INTENSET_DRE;
		xSemaphoreTake(spi_semaphore, portMAX_DELAY);
	}
}

static inline uint8_t ENC28J60_SERCOM_writeread(uint8_t w) {
	
	spi_context.operation = WriteRead;
	*ENC28J60_SPI_DATA = w;
	*ENC28J60_SPI_INTENSET = SERCOM_SPI_INTENSET_DRE;
	xSemaphoreTake(spi_semaphore, portMAX_DELAY);

	return spi_context.read_data;
}

static inline uint8_t ENC28J60_SERCOM_writeread_macmii(uint8_t w) {
	
	spi_context.macmii_data[0] = 0;
	spi_context.macmii_data[1] = 0;

	spi_context.read_buffer = spi_context.macmii_data;
	spi_context.read_length = 2;

	spi_context.operation = WriteRead_MacMii;
	*ENC28J60_SPI_DATA = w;
	*ENC28J60_SPI_INTENSET = SERCOM_SPI_INTENSET_DRE;
	xSemaphoreTake(spi_semaphore, portMAX_DELAY);

	return spi_context.macmii_data[0];	// FIXME
}

static inline void ENC28J60_SERCOM_readBuffer(uint8_t *buf, uint16_t len) {

	spi_context.operation = ReadBuffer;
	spi_context.read_buffer = buf;
	spi_context.read_length = len;
	*ENC28J60_SPI_DATA = rbm_inst;
	*ENC28J60_SPI_INTENSET = SERCOM_SPI_INTENSET_DRE;
	xSemaphoreTake(spi_semaphore, portMAX_DELAY);
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

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	v = ENC28J60_SERCOM_writeread(buf);
	ENC28J60_NCS_HIGH();

	return v;
}

uint8_t ENC_ReadCtrl8_MacMii(enc28j60_registers_t a) {
	uint8_t v;
	uint8_t buf = rcr_inst | (a & SFR_MASK);

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	v = ENC28J60_SERCOM_writeread_macmii(buf);
	ENC28J60_NCS_HIGH();

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

	uint16_t v = ((uint16_t)b2 << 8 | b1);

	return v;
}

uint16_t ENC_ReadCtrl16_MacMii(enc28j60_registers_t a) {
	uint8_t b1, b2;
	uint8_t buf = rcr_inst | (a);

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

	uint16_t v = ((uint16_t)b2 << 8 | b1);

	return v;
}

/**
 * Write 1 byte to ENC
 * @param v
 */
void ENC_Write8(uint8_t v)
{
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(&v, 1);
	ENC28J60_NCS_HIGH();
}

/**
 * Write 1 byte to SFRs
 * @param a
 * @param v
 */
void ENC_WriteCtrl8(enc28j60_registers_t a, uint8_t v)
{
	uint8_t buf[2] = {(wcr_inst | (a & SFR_MASK)), v};

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(buf, 2);
	ENC28J60_NCS_HIGH();
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

	ENC_BankSel(a);
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(buf, 2);
	ENC28J60_NCS_HIGH();
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
	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_readBuffer(buffer, length);
	ENC28J60_NCS_HIGH();
}

/**
 * Write a block of data to TX Buffer
 * @param data
 * @param length
 */
void ENC_WriteBuffer(uint8_t *data, uint16_t length)
{
	uint8_t buf = wbm_inst;

	ENC28J60_NCS_LOW();
	ENC28J60_SERCOM_write(&buf, 1);
	ENC28J60_SERCOM_write(data, length);
	ENC28J60_NCS_HIGH();
}

void ENC_RegisterIrq(void* irq) {
	ENC_InitIRQ();

	ext_irq_register(ENC28J60_PIN_IRQ, irq);

	irq_callback = irq;
}

void ENC28J60_SERCOM_HANDLER(void) {
	uint8_t intflag = *ENC28J60_SPI_INTFLAG;
	uint8_t intset = *ENC28J60_SPI_INTENSET;

	if((intflag & SERCOM_SPI_INTFLAG_DRE) && (intset & SERCOM_SPI_INTENSET_DRE)) {
		switch(spi_context.operation) {
			case Write:
				if(spi_context.write_length) {
					*ENC28J60_SPI_DATA = *spi_context.write_buffer++;
					spi_context.write_length--;
				} else {
					*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_DRE;
					*ENC28J60_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
					spi_context.operation = None;
					xSemaphoreGiveFromISR(spi_semaphore, NULL);
				}
				break;
			case WriteRead:
			case WriteRead_MacMii:
			case ReadBuffer:
				*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_DRE;
				*ENC28J60_SPI_INTENSET = SERCOM_SPI_INTENSET_RXC;
				*ENC28J60_SPI_CTRLB = SERCOM_SPI_CTRLB_RXEN;
				*ENC28J60_SPI_DATA = 0xFF;	// Shift dummy byte to read
				break;
			default:
				*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_DRE;
				spi_context.operation = None;
				xSemaphoreGiveFromISR(spi_semaphore, NULL);
				break;
		}
	} else if((intflag & SERCOM_SPI_INTFLAG_TXC) && (intset & SERCOM_SPI_INTENSET_TXC)) {
		switch(spi_context.operation) {
			case Write:
				*ENC28J60_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
				*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_TXC;
				spi_context.operation = None;
				xSemaphoreGiveFromISR(spi_semaphore, NULL);
				break;
			default:
				*ENC28J60_SPI_INTFLAG = SERCOM_SPI_INTFLAG_TXC;
				*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_TXC;
				spi_context.operation = None;
				xSemaphoreGiveFromISR(spi_semaphore, NULL);
				break;
		}
	} else if((intflag & SERCOM_SPI_INTFLAG_RXC) && (intset & SERCOM_SPI_INTENSET_RXC)) {
		switch(spi_context.operation) {
			case WriteRead:
				spi_context.read_data = *ENC28J60_SPI_DATA;
				*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_RXC;
				*ENC28J60_SPI_CTRLB = 0;
				spi_context.operation = None;
				xSemaphoreGiveFromISR(spi_semaphore, NULL);
				break;
			case WriteRead_MacMii:
			case ReadBuffer:
				*spi_context.read_buffer++ = *ENC28J60_SPI_DATA;
				if(--spi_context.read_length) {
					*ENC28J60_SPI_DATA = 0xFF;	// Shift dummy byte to read
				} else {
					*ENC28J60_SPI_INTENCLR = SERCOM_SPI_INTENCLR_RXC;
					*ENC28J60_SPI_CTRLB = 0;
					spi_context.operation = None;
					xSemaphoreGiveFromISR(spi_semaphore, NULL);
				}
				break;
			default:
				spi_context.read_data = *ENC28J60_SPI_DATA;
				*ENC28J60_SPI_CTRLB = 0;
				spi_context.operation = None;
				xSemaphoreGiveFromISR(spi_semaphore, NULL);
				break;
		}
	}
}

void EIC_Handler(void) {

	EIC->INTFLAG.reg = 0xFFFFFFFF;

	if(irq_callback != NULL) {
		irq_callback();
	}
}