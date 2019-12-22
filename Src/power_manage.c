#include "power_manage.h"

int32_t PowerManageInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOC)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    
    LL_GPIO_InitTypeDef powerManageGpioInit;
    LL_GPIO_StructInit(&powerManageGpioInit);
    powerManageGpioInit.Pin = LL_GPIO_PIN_7;
    powerManageGpioInit.Mode = LL_GPIO_MODE_OUTPUT;
    powerManageGpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    powerManageGpioInit.Pull = LL_GPIO_PULL_UP;
    powerManageGpioInit.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOC, &powerManageGpioInit);
    powerManageGpioInit.Pin = LL_GPIO_PIN_8;
    powerManageGpioInit.Mode = LL_GPIO_MODE_INPUT;
    powerManageGpioInit.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOC, &powerManageGpioInit);

    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);

    UsartSetMystdioHandler(USART2);
    MyPrintf("Power manage initialization succeed.\n");
    return 0;
}

//this function enable 5V device
int32_t DeviceEnable(){
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_7);
    UsartSetMystdioHandler(USART2);
    MyPrintf("Enable 5V device.\n");
    return 0; 
}

int32_t DeviceDisable(){
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);
    UsartSetMystdioHandler(USART2);
    MyPrintf("Disable 5V device.\n");
    return 0;
}
