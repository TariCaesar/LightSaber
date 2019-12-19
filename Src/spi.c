#include "spi.h"

static uint8_t spiDmaTxDummy = 0xff;
static uint8_t spiDmaRxDummy = 0xff;

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
 
    LL_SPI_EnableDMAReq_TX(SPI2);
    LL_SPI_EnableDMAReq_RX(SPI2);

    //dma init
    //enable dma clock
    if(!LL_APB2_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
        LL_APB2_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    }
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
    NVIC_SetPriority(DMA1_Channel4_IRQn, NVIC_EncodePriority(2, 3, 3));
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
    NVIC_SetPriority(DMA1_Channel5_IRQn, NVIC_EncodePriority(2, 3, 3));
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);


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

uint32_t SpiWriteReadDMA(uint8_t* addrSrc, uint8_t* addrDst, uint32_t size){
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, size);
    if(addrSrc){
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_5,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
            LL_DMA_PRIORITY_MEDIUM |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
        );
        LL_DMA_ConfigAddresses(
            DMA1,
            LL_DMA_CHANNEL_5,
            (uint32_t)addrSrc,
            LL_SPI_DMA_GetRegAddr(SPI2),
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5)
        );
    }
    else{
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_5,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
            LL_DMA_PRIORITY_MEDIUM |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
        );
        LL_DMA_ConfigAddresses(
            DMA1,
            LL_DMA_CHANNEL_5,
            (uint32_t)&spiDmaTxDummy,
            LL_SPI_DMA_GetRegAddr(SPI2),
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5)
        );
    }

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, size);
    if(addrDst){
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_4,
            LL_DMA_DIRECTION_PERIPH_TO_MEMORY|
            LL_DMA_PRIORITY_MEDIUM |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
        );
        LL_DMA_ConfigAddresses(
            DMA1,
            LL_DMA_CHANNEL_4,
            LL_SPI_DMA_GetRegAddr(SPI2),
            (uint32_t)addrDst,
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4)
        );
    }
    else{
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_4,
            LL_DMA_DIRECTION_PERIPH_TO_MEMORY|
            LL_DMA_PRIORITY_MEDIUM |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
        );
        LL_DMA_ConfigAddresses(
            DMA1,
            LL_DMA_CHANNEL_4,
            LL_SPI_DMA_GetRegAddr(SPI2),
            (uint32_t)&spiDmaRxDummy,
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4)
        );
    }

    SpiSSEnable();
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    return size;
}

void DMA1_Channel4_IRQHandler(){
    LL_DMA_ClearFlag_TC4(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    SpiSSDisable();
    return;
}

void DMA1_Channel5_IRQHandler(){
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    return;
}
