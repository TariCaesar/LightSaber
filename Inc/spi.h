#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "mystdio.h"
#include "usart.h"

__STATIC_INLINE void SpiEnable()
{
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) & (~(0x1 << 12)));
    LL_SPI_Enable(SPI2);
}

__STATIC_INLINE void SpiDisable()
{
    LL_SPI_Disable(SPI2);
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) | (0x1 << 12));
}

int32_t SpiInit();

//must enable spi before this
//disable spi mannually if you want to end this frame transfer
uint8_t SpiWriteReadByte(uint8_t dataWrite);

//for useless data use NULL or 0 as input parameter
//don't enable spi as it will do inside
//but need disable spi in handler if use handler
int32_t SpiWriteReadByteIT(uint8_t dataTx, uint8_t* addrDst, void (*handler)(void));
#endif
