#ifndef I2C_H
#define I2C_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_i2c.h"

#define I2C_REQUEST_WRITE 0xfe
#define I2C_REQUEST_READ 0x01

int32_t I2cInit();
uint32_t I2cWrite(uint8_t deviceAddr, uint8_t* addrSrc, uint32_t dataSize);
uint8_t I2cRead(uint8_t deviceAddr, uint8_t regAddr);
#endif
