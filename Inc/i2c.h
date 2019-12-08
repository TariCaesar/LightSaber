#ifndef I2C_H
#define I2C_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_i2c.h"
#include "mystdio.h"
#include "usart.h"

#define I2C_REQUEST_WRITE 0xfe
#define I2C_REQUEST_READ 0x01

typedef struct{
    uint8_t deviceAddr;
    uint8_t* addr;
    uint32_t direction;
}I2C_TASK;

int32_t I2cInit();
uint8_t I2cWriteByte(uint8_t deviceAddr, uint8_t data);
//i2cWriteWord first transmit high byte and then low byte
uint16_t I2cWriteHalfWord(uint8_t deviceAddr, uint16_t data);
uint8_t I2cReadByte(uint8_t deviceAddr);
int32_t I2cReadByteIT(uint8_t deviceAddr, uint8_t* addrDst);
int32_t I2cWriteByteIT(uint8_t deviceAddr, uint8_t* addrSrc);
int32_t I2cTransferWrap(I2C_TASK* task, uint32_t taskNum);

#endif
