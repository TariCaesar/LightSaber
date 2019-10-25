#include "spi.h"

int SpiInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    LL_GPIO_InitTypeDef spi2GpioInit;
    spi2GpioInit.Pin = LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_15;
    spi2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    spi2GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    spi2GpioInit.Speed = LL_GPIO_SPEED_FREQ_LOW;
    spi2GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &spi2GpioInit);
    return 0;
}