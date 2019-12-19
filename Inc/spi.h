#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_dma.h"
#include "mystdio.h"
#include "usart.h"

__STATIC_INLINE void SpiSSEnable()
{
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
}

__STATIC_INLINE void SpiSSDisable()
{
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
}

__STATIC_INLINE int32_t SpiSSIsEnabled()
{
    return !(LL_GPIO_ReadInputPort(GPIOA) & (0x1 << 15));
}

int32_t SpiInit();

//must enable spi before this
//disable spi mannually if you want to end this frame transfer
uint8_t SpiWriteReadByte(uint8_t dataWrite);

//for useless data use NULL or 0 as input parameter
uint32_t SpiWriteReadDMA(uint8_t* addrSrc, uint8_t* addrDst, uint32_t size, void (*callbackHandler)(void));
#endif
