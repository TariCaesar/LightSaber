#include "dac.h"

static int16_t* audioData;
static uint32_t audioDataNum = 0;
static uint32_t audioDataCnt = 0;
static void (*audioPlayEndHandler)(void) = 0;

int32_t IRQcnt = 0;

void DacUpdateHandler(){
    if(audioDataCnt < audioDataNum){
        int32_t dataOffset = ((int32_t)(audioData[audioDataCnt])) / 32  + 2048;

        LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, (uint16_t)dataOffset);
        audioDataCnt += 1;
        IRQcnt += 1;
    }
    else{
        Timer3Disable();
        if(audioPlayEndHandler == 0){
            LL_DAC_Disable(DAC1, LL_DAC_CHANNEL_1);
            LL_DAC_Disable(DAC1, LL_DAC_CHANNEL_2);
        }
        else{
            audioPlayEndHandler();
        }
    }
    return;
}

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
    
    LL_DAC_DeInit(DAC1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);
    LL_DAC_InitTypeDef dacInit;
    LL_DAC_StructInit(&dacInit);
    dacInit.OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE;
    dacInit.TriggerSource = LL_DAC_TRIG_SOFTWARE;
    dacInit.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE;

    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &dacInit);
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_2, &dacInit);
    
    Timer3Init(DacUpdateHandler, 44100);

    UsartSetMystdioHandler(USART2);
    MyPrintf("DAC initialization succeed!\n");
    return 0;
}

uint32_t DacAudioPlay(int16_t* data, uint32_t size, void (*handler)(void)){
    audioDataNum = size;
    audioDataCnt = 0;
    audioData = data;
    audioPlayEndHandler = handler;
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_2);
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_2, 2048);
    Timer3Enable();
    return size;
}

