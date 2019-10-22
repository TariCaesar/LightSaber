#include "stm32f103xb.h"
#include "stm32f1xx_ll_gpio.h"

int main(){
    LL_GPIO_InitTypeDef GPIOInit;
    GPIOInit.Pin = LL_GPIO_PIN_0;
    GPIOInit.Mode = LL_GPIO_MODE_OUTPUT;
    GPIOInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIOInit.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOC, &GPIOInit);
    LL_GPIO_WriteOutputPort(GPIOC, 0xfffe);

    return 0;
}
