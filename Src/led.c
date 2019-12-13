#include "led.h"

int32_t LedInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOC)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    
    LL_GPIO_InitTypeDef ledGpioInit;
    LL_GPIO_StructInit(&ledGpioInit);
    ledGpioInit.Mode = LL_GPIO_MODE_OUTPUT;
    ledGpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    ledGpioInit.Pin = LL_GPIO_PIN_0;
    ledGpioInit.Pull = LL_GPIO_PULL_UP;
    ledGpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOC, &ledGpioInit);
    
    
}
