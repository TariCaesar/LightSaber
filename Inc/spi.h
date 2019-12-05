#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"

__STATIC_INLINE void SpiSSEnable()
{
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) & (~(0x1 << 12)));
}

__STATIC_INLINE void SpiSSDisable()
{
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) | (0x1 << 12));
}

int32_t SpiInit();
uint8_t SpiWriteReadByte(uint8_t dataWrite);
#endif
