#ifndef I2C_H
#define I2C_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_i2c.h"
#include "mystdio.h"

#define I2C_REQUEST_WRITE 0xfe
#define I2C_REQUEST_READ 0x01

int32_t I2cInit();
uint8_t I2cWriteByte(uint8_t deviceAddr, uint8_t regAddr, uint8_t data);
uint8_t I2cReadByte(uint8_t deviceAddr, uint8_t regAddr);
#endif
