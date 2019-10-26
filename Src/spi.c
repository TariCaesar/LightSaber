#include "spi.h"

int SpiInit(){
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    LL_GPIO_InitTypeDef spi2GpioInit;
    spi2GpioInit.Pin = LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    spi2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    spi2GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    spi2GpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    spi2GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);
    spi2GpioInit.Pin = LL_GPIO_PIN_14;
    spi2GpioInit.Mode = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);

    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    LL_SPI_InitTypeDef spi2Init;
    LL_SPI_StructInit(&spi2Init);
    spi2Init.Mode = LL_SPI_MODE_MASTER;
    spi2Init.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    //Set prescale=2, means spi2 clock=18MHz
    spi2Init.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
    spi2Init.BitOrder= LL_SPI_MSB_FIRST;
    spi2Init.ClockPhase = LL_SPI_PHASE_2EDGE;
    spi2Init.ClockPolarity = LL_SPI_POLARITY_HIGH;
    spi2Init.TransferDirection = LL_SPI_FULL_DUPLEX;
    spi2Init.NSS = LL_SPI_NSS_HARD_OUTPUT;
    LL_SPI_Init(SPI2, &spi2Init);

    //Enable SPI2
    LL_SPI_Enable(SPI2);
    
    return 0;
}