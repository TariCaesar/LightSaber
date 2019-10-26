#ifndef SPI_H
#define SPI_H

#include "stm32f103xb.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"

int32_t SpiInit();
uint8_t SpiWriteReadByte(uint8_t dataWrite);
#endif