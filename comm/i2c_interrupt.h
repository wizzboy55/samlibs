/*
 * i2c_interrupt.h
 *
 * Created: 2022-03-19 10:00:23 AM
 *  Author: charl
 */ 


#ifndef I2C_INTERRUPT_H_
#define I2C_INTERRUPT_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef enum {I2C_WRITE, I2C_READ, I2C_WRITEREAD} I2CTransactionType_t;

typedef enum {I2C_IDLE, I2C_WRITE_REGISTER, I2C_WRITE_DATA, I2C_READ_DATA} I2CState_t;

typedef struct {
	uint8_t ucDeviceAddress;
	uint8_t ucRegisterAddress;
	uint8_t* pcData;
	uint8_t ucDataLength;
	uint8_t ucDataCounter;
	I2CTransactionType_t xTransactionType;
	BaseType_t xTransactionSuccess;
	TaskHandle_t xRequestingTask;
} I2CTransaction_t;

typedef struct {
	void* sercom;
	uint32_t baudrate;
	uint8_t pin_sda;
	uint8_t pin_scl;
	uint8_t pinmux;
	uint8_t clksource;
	TickType_t xTimeout;
	BaseType_t xBigEndian;
	UBaseType_t uxPriority;
	QueueHandle_t xTransactionQueue;
	UBaseType_t uxQueueLength;
	I2CTransaction_t xCurrentTransaction;
	TaskHandle_t xI2cTask;
	I2CState_t xState;
} I2CInterruptConfig_t;

void vI2cInterruptInit(I2CInterruptConfig_t* config);
void vI2cInterruptMBRoutine(I2CInterruptConfig_t* config);
void vI2cInterruptSBRoutine(I2CInterruptConfig_t* config);
void vI2cInterruptErrorRoutine(I2CInterruptConfig_t* config);

#endif /* I2C_INTERRUPT_H_ */