#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "mystdio.h"
#include "usart.h"

__STATIC_INLINE void SpiSSEnable()
{
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) & (~(0x1 << 12)));
}

__STATIC_INLINE void SpiSSDisable()
{
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) | (0x1 << 12));
}

int32_t SpiInit();

//must enable spi before this
//disable spi mannually if you want to end this frame transfer
uint8_t SpiWriteReadByte(uint8_t dataWrite);

//for useless data use NULL or 0 as input parameter
//don't enable spi, it will do it inside
//but need disable spi in handler if you want to end this transfer frame
//or use 0 as handler, as it will automatically end this transfer frame if handler equal to 1
int32_t SpiWriteReadByteIT(uint8_t dataTx, uint8_t* addrDst, void (*handler)(void));
#endif
