#ifndef SPI_H
#define SPI_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_dma.h"
#include "mystdio.h"
#include "usart.h"

__STATIC_INLINE void Spi1SSEnable()
{
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);
}

__STATIC_INLINE void Spi1SSDisable()
{
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_15);
}

__STATIC_INLINE int32_t Spi1SSIsEnabled()
{
    return !(LL_GPIO_ReadInputPort(GPIOA) & (0x1 << 15));
}

int32_t Spi1Init();

int32_t Spi2Init();

//must enable spi ss before this
//disable spi mannually if you want to end this frame transfer
uint8_t Spi1WriteReadByte(uint8_t dataWrite);

//for useless data use NULL or 0 as input parameter
uint32_t Spi1WriteReadDMA(uint8_t* addrSrc, uint8_t* addrDst, uint32_t size, void (*callbackHandler)(void));

int32_t Spi2WriteDma(uint8_t* addrSrc, uint32_t size, void (*callbackHandler)(void));

//use dma transfer with noincrement data src
int32_t Spi2WriteDummyDma(uint8_t* addrSrc, uint32_t size, void (*callbackHandler)(void));
#endif
