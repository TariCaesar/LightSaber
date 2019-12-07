#ifdef STM32F103xE
#include "dac.h"

int32_t DacInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOA))
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef dacGpioInit;
    LL_GPIO_StructInit(&dacGpioInit);
    dacGpioInit.Mode = LL_GPIO_MODE_ANALOG;
    dacGpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    dacGpioInit.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_5;
    dacGpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOA, &dacGpioInit);
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
    LL_DAC_InitTypeDef dacInit;
    LL_DAC_StructInit(&dacInit);
    dacInit.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;
    dacInit.TriggerSource = LL_DAC_TRIG_SOFTWARE;
    dacInit.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE;

    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &dacInit);
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_2, &dacInit);

    

    SetMystdioTarget(USART2);
    MyPrintf("DAC init success!\n");
}

#endif
