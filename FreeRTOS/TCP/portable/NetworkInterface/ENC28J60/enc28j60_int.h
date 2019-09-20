/*
 * enc28j60.h
 *
 * Created: 2019-01-04 9:23:03 AM
 *  Author: Wizz
 */ 


#ifndef ENC28J60_H_
#define ENC28J60_H_

#include "enc28j60_registers.h"
#include <stdint.h>

void ENC_IfInit(void);

uint8_t  ENC_ReadCtrl8(enc28j60_registers_t a);
uint8_t	 ENC_ReadCtrl8_MacMii(enc28j60_registers_t a);
uint16_t ENC_ReadCtrl16(enc28j60_registers_t a);
uint16_t ENC_ReadCtrl16_MacMii(enc28j60_registers_t a);

void ENC_WriteCtrl8(enc28j60_registers_t a, uint8_t v);
void ENC_WriteCtrl16(enc28j60_registers_t a, uint16_t v);

uint16_t ENC_ReadPhy(enc28j60_phy_registers_t a);
void     ENC_WritePhy(enc28j60_phy_registers_t a, uint16_t d);

void ENC_Write8(uint8_t);
void ENC_WriteOp(spi_inst_t op, enc28j60_registers_t a, uint8_t d);

void ENC_WriteBuffer(uint8_t *data, uint16_t length);
void ENC_ReadBuffer(uint8_t *buffer, uint16_t length);

void ENC_RegisterIrq(void* irq);

#endif /* ENC28J60_H_ */