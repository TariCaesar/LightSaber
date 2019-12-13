#include "spi.h"

static struct{
    uint8_t dataTx;
    uint8_t* addrDst;
    void (*handler)(void);
    int32_t active;
}spiTask;

int32_t SpiInit()
{
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    LL_GPIO_InitTypeDef spi2GpioInit;
    //PB13 is CLK for spi, PB15 is MOSI, configure into af output
    spi2GpioInit.Pin = LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    spi2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    spi2GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    spi2GpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    spi2GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);
    //PB14 is MISO for spi, configure into input
    spi2GpioInit.Pin = LL_GPIO_PIN_14;
    spi2GpioInit.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);
    //PB12 is SS for spi, configure into push-pull output
    spi2GpioInit.Pin = LL_GPIO_PIN_12;
    spi2GpioInit.Mode = LL_GPIO_MODE_OUTPUT;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);
    LL_GPIO_WriteOutputPort(GPIOB, LL_GPIO_ReadOutputPort(GPIOB) | 0x1000);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_SPI_DeInit(SPI2);
    LL_SPI_InitTypeDef spi2Init;
    LL_SPI_StructInit(&spi2Init);
    spi2Init.Mode = LL_SPI_MODE_MASTER;
    spi2Init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    spi2Init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    spi2Init.BitOrder = LL_SPI_MSB_FIRST;
    spi2Init.ClockPhase = LL_SPI_PHASE_2EDGE;
    spi2Init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    spi2Init.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi2Init.NSS = LL_SPI_NSS_SOFT;
    LL_SPI_Init(SPI2, &spi2Init);
 
    //Config SPI interrupt priority for DMA
    NVIC_SetPriority(SPI2_IRQn, NVIC_EncodePriority(2, 2, 2));
    NVIC_EnableIRQ(SPI2_IRQn);

    spiTask.active = 0;

    LL_SPI_Enable(SPI2);
    UsartSetMystdioHandler(USART2);
    MyPrintf("SPI initialization succeed!\n");
    return 0;
}

uint8_t SpiWriteReadByte(uint8_t dataWrite)
{
    if(LL_SPI_IsActiveFlag_BSY(SPI2))return 0;
    while(!LL_SPI_IsActiveFlag_TXE(SPI2))continue;
    LL_SPI_TransmitData8(SPI2, dataWrite);
    while(!LL_SPI_IsActiveFlag_RXNE(SPI2))continue;
    uint8_t data = LL_SPI_ReceiveData8(SPI2);
    return data;
}

int32_t SpiWriteReadByteIT(uint8_t dataTx, uint8_t* addrDst, void (*handler)(void)){
    //enable spi interrupt 
    spiTask.dataTx = dataTx;
    spiTask.addrDst = addrDst;
    spiTask.handler = handler;
    spiTask.active = 1;
    SpiSSEnable();
    LL_SPI_EnableIT_RXNE(SPI2);
    LL_SPI_EnableIT_TXE(SPI2);
    return 0;
}

void SPI2_IRQHandler(){
    if(!spiTask.active)return;
    else{
        if(LL_SPI_IsActiveFlag_RXNE(SPI2)){
            if(spiTask.addrDst == 0)LL_SPI_ReceiveData8(SPI2);
            else *(spiTask.addrDst) = LL_SPI_ReceiveData8(SPI2);
            LL_SPI_DisableIT_RXNE(SPI2);
            spiTask.active = 0;
            if(spiTask.handler == 0)SpiSSDisable();
            else spiTask.handler();
            return;
        }
        else if(LL_SPI_IsActiveFlag_TXE(SPI2)){
            LL_SPI_TransmitData8(SPI2, spiTask.dataTx);
            LL_SPI_DisableIT_TXE(SPI2);
            return;
        }
    }
}
