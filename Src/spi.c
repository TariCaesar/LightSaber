#include "spi.h"

static uint8_t spi1DmaTxDummy = 0xff;
static uint8_t spi1DmaRxDummy = 0xff;
static void (*spi1DmaCallbackHandler)(void) = 0;

int32_t Spi1Init()
{
    //spi1 for flash
    //check the GPIOB and GPIOA clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    }
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_AFIO)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    }
    LL_GPIO_AF_EnableRemap_SPI1();

    LL_GPIO_InitTypeDef spi1GpioInit;
    //After remap, PB3 is CLK for spi, PB5 is MOSI, configure into af output
    spi1GpioInit.Pin = LL_GPIO_PIN_3 | LL_GPIO_PIN_5;
    spi1GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    spi1GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    spi1GpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    spi1GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &spi1GpioInit);
    //After remap, PA15 is SS for spi, configure into output
    spi1GpioInit.Pin = LL_GPIO_PIN_15;
    spi1GpioInit.Mode = LL_GPIO_MODE_OUTPUT;
    LL_GPIO_Init(GPIOA, &spi1GpioInit);
    //After remap, PB4 is MISO for spi, configure into input
    spi1GpioInit.Pin = LL_GPIO_PIN_4;
    spi1GpioInit.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOB, &spi1GpioInit);

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);

    LL_SPI_DeInit(SPI1);
    LL_SPI_InitTypeDef spi1Init;
    LL_SPI_StructInit(&spi1Init);
    spi1Init.Mode = LL_SPI_MODE_MASTER;
    spi1Init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    spi1Init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    spi1Init.BitOrder = LL_SPI_MSB_FIRST;
    spi1Init.ClockPhase = LL_SPI_PHASE_2EDGE;
    spi1Init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    spi1Init.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi1Init.NSS = LL_SPI_NSS_SOFT;
    LL_SPI_Init(SPI1, &spi1Init);
 

    //dma init
    //enable dma clock
    if(!LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    }
    LL_DMA_ClearFlag_TC2(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
    NVIC_SetPriority(DMA1_Channel2_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
    NVIC_SetPriority(DMA1_Channel3_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);

    LL_SPI_Enable(SPI1);
    UsartSetMystdioHandler(USART2);
    MyPrintf("SPI1 initialization succeed!\n");
    return 0;
}

int32_t Spi2Init(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    //SPI2 for led 
    LL_GPIO_InitTypeDef spi2GpioInit;
    LL_GPIO_StructInit(&spi2GpioInit);
    spi2GpioInit.Pin = LL_GPIO_PIN_15;
    spi2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    spi2GpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    spi2GpioInit.Pull = LL_GPIO_PULL_UP;
    spi2GpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_SPI_DeInit(SPI2);
    LL_SPI_InitTypeDef spi2Init;
    LL_SPI_StructInit(&spi2Init);
    spi2Init.Mode = LL_SPI_MODE_MASTER;
    spi2Init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    //spi2 has 6MHz frequency
    spi2Init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    spi2Init.BitOrder = LL_SPI_MSB_FIRST;
    spi2Init.ClockPhase = LL_SPI_PHASE_2EDGE;
    spi2Init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    spi2Init.TransferDirection = LL_SPI_HALF_DUPLEX_TX;
    spi2Init.NSS = LL_SPI_NSS_SOFT;
    LL_SPI_Init(SPI2, &spi2Init);

    
    if(!LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    }

    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
    NVIC_SetPriority(DMA1_Channel5_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    LL_SPI_Enable(SPI2);

    UsartSetMystdioHandler(USART2);
    MyPrintf("SPI2 initialization succeed!\n");
    return 0;
}

uint8_t Spi1WriteReadByte(uint8_t dataWrite)
{
    while(!LL_SPI_IsActiveFlag_TXE(SPI1))continue;
    LL_SPI_TransmitData8(SPI1, dataWrite);
    while(!LL_SPI_IsActiveFlag_RXNE(SPI1))continue;
    return LL_SPI_ReceiveData8(SPI1);
}

uint32_t Spi1WriteReadDMA(uint8_t* addrSrc, uint8_t* addrDst, uint32_t size, void (*callbackHandler)(void)){
    spi1DmaCallbackHandler = callbackHandler;
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, size);
    if(addrSrc){
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_3,
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
            LL_DMA_CHANNEL_3,
            (uint32_t)addrSrc,
            LL_SPI_DMA_GetRegAddr(SPI1),
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3)
        );
    }
    else{
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_3,
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
            LL_DMA_CHANNEL_3,
            (uint32_t)&spi1DmaTxDummy,
            LL_SPI_DMA_GetRegAddr(SPI1),
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3)
        );
    }

    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, size);
    if(addrDst){
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_2,
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
            LL_DMA_CHANNEL_2,
            LL_SPI_DMA_GetRegAddr(SPI1),
            (uint32_t)addrDst,
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2)
        );
    }
    else{
        LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_2,
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
            LL_DMA_CHANNEL_2,
            LL_SPI_DMA_GetRegAddr(SPI1),
            (uint32_t)&spi1DmaRxDummy,
            LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2)
        );
    }

    if(!Spi1SSIsEnabled())Spi1SSEnable();
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_SPI_EnableDMAReq_TX(SPI1);
    LL_SPI_EnableDMAReq_RX(SPI1);
    return size;
}

int32_t Spi2DMATrigger(uint8_t* addrSrc, uint32_t size){
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_5,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
            LL_DMA_PRIORITY_VERYHIGH |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
    );
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, size);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_CHANNEL_5,
        (uint32_t)addrSrc,
        LL_SPI_DMA_GetRegAddr(SPI2),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5)
    );
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_SPI_EnableDMAReq_TX(SPI2);
    return 0;
}

int32_t Spi2DMATriggerDummy(uint8_t* addrSrc, uint32_t size){
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_5,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
            LL_DMA_PRIORITY_VERYHIGH |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_NOINCREMENT |
            LL_DMA_PDATAALIGN_BYTE |
            LL_DMA_MDATAALIGN_BYTE
    );
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, size);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_CHANNEL_5,
        (uint32_t)addrSrc,
        LL_SPI_DMA_GetRegAddr(SPI2),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5)
    );
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_SPI_EnableDMAReq_TX(SPI2);
    return 0;
}

void DMA1_Channel2_IRQHandler(){
    LL_DMA_ClearFlag_TC2(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_SPI_DisableDMAReq_TX(SPI1);
    LL_SPI_DisableDMAReq_RX(SPI1);
    if(spi1DmaCallbackHandler == 0)Spi1SSDisable();
    else spi1DmaCallbackHandler();
    return;
}

void DMA1_Channel3_IRQHandler(){
    LL_DMA_ClearFlag_TC3(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    return;
}

void DMA1_Channel5_IRQHandler(){
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
    LL_SPI_DisableDMAReq_TX(SPI2);
    return;
}
