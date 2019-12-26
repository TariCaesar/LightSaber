#include "power_manage.h"

volatile int32_t systemNeedShutdown = 0;
volatile int32_t shutdownAudioPlayed = 0;

void ShutdownAudioCallbackHandler(){
    systemNeedShutdown = 0;
    shutdownAudioPlayed = 1;
}

static inline int32_t IsPowerButtonPressed(){
    return LL_GPIO_ReadInputPort(GPIOC) & (0x1 << 8);
}

//this function enable 5V device
static int32_t DeviceEnable(){
    LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_7);
    UsartSetMystdioHandler(USART2);
    MyPrintf("Enable 5V device.\n");
    return 0; 
}

static int32_t DeviceDisable(){
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);
    UsartSetMystdioHandler(USART2);
    MyPrintf("Disable 5V device.\n");
    return 0;
}

int32_t EnterStopMode(){
    DeviceDisable();
    //Wait for usart transfer complete
    while(!UsartTxBufferIsEmpty(USART1) || !UsartTxBufferIsEmpty(USART2))continue;
    PeripheralStop();
    //disable all interrupt
    InterruptDisable();
    LL_PWR_SetPowerMode(LL_PWR_MODE_STOP_MAINREGU);
    LL_LPM_EnableDeepSleep();
    __WFI();
    return 0;
}

int32_t PowerManageInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOC)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_AFIO)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
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
    LL_GPIO_AF_SetEXTISource(LL_GPIO_AF_EXTI_PORTC, LL_GPIO_AF_EXTI_LINE8);
    LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_7);

    LL_EXTI_InitTypeDef powerExtiInit;
    LL_EXTI_StructInit(&powerExtiInit);
    powerExtiInit.Line_0_31 = LL_EXTI_LINE_8;
    powerExtiInit.LineCommand = ENABLE;
    powerExtiInit.Mode = LL_EXTI_MODE_IT;
    powerExtiInit.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
    LL_EXTI_Init(&powerExtiInit);

    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_8);
    NVIC_SetPriority(EXTI9_5_IRQn, NVIC_EncodePriority(2, 0, 2));
    NVIC_EnableIRQ(EXTI9_5_IRQn);
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    systemNeedShutdown = 0;
    shutdownAudioPlayed = 0;

    UsartSetMystdioHandler(USART2);
    MyPrintf("Power manage initialization succeed.\n");
    return 0;
}


int32_t PowerBootManage(){
    if(IsPowerButtonPressed()){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Power Button is pressed.\n");
        MyPrintf("Run normally.\n");
        DeviceEnable();
        return 1;
    }
    else{
        UsartSetMystdioHandler(USART2);
        MyPrintf("Power Button is not pressed.\n");
        MyPrintf("Prepare to shutdown.\n");
        EnterStopMode();
        return 0;
    }
}


 
void EXTI9_5_IRQHandler(){
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
    if(IsPowerButtonPressed()){
        NVIC_SystemReset();
    }
    else{
        UsartSetMystdioHandler(USART2);
        MyPrintf("Power button not pressed.\n");
        MyPrintf("Prepare to shutdown.\n");
        systemNeedShutdown = 1;
    }
}
