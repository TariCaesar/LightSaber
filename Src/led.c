#include "led.h"

LED_COLOR ledColorData[LED_NUM];
static int32_t ledColorDataUnroll[LED_NUM * 24];

static int16_t highThTick;  
static int16_t lowThTick;  
static int16_t highTlTick;  
static int16_t lowTlTick;  

static int32_t ledColorState;
static int32_t ledColorPhase = 0;
static int32_t ledColorUpdateCnt = 0;

static inline LedWriteHigh(){
    LL_GPIO_WriteOutputPort(GPIOC, 0x1);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    LL_GPIO_WriteOutputPort(GPIOC, ~0x1);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

static inline LedWriteLow(){
    LL_GPIO_WriteOutputPort(GPIOC, ~0x1);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    LL_GPIO_WriteOutputPort(GPIOC, 0x1);
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
}

void LedUpdateHandler(){
    if(ledColorPhase){
        LL_GPIO_WriteOutputPort(GPIOC, LL_GPIO_ReadOutputPort(GPIOC) | (0x1));
        if(ledColorState){
            Timer4Enable(highThTick);
        }
        else{
            Timer4Enable(lowThTick);
        }
    }
    else{
        LL_GPIO_WriteOutputPort(GPIOC, LL_GPIO_ReadOutputPort(GPIOC) & ~(0x1));
        if(ledColorState){
            Timer4Enable(highTlTick);
        }
        else{
            Timer4Enable(lowTlTick);
        }
        //fetch next
        ledColorUpdateCnt += 1;
        ledColorState = ledColorDataUnroll[ledColorUpdateCnt];
    }
    ledColorPhase = !ledColorPhase;
    if(ledColorUpdateCnt == LED_NUM * 24)
        Timer4Disable();
    return;
}

int32_t LedUpdate(){
    for(int32_t i = 0; i < LED_NUM; ++i){
        for(int32_t j = 0; j < LED_NUM; ++j){
            ledColorDataUnroll[i * 24 + j] = ledColorData[i].G & (0x80 >> j);
        }
        for(int32_t j = 0; j < LED_NUM; ++j){
            ledColorDataUnroll[i * 24 + j + 8] = ledColorData[i].R & (0x80 >> j);
        }
        for(int32_t j = 0; j < LED_NUM; ++j){
            ledColorDataUnroll[i * 24 + j + 16] = ledColorData[i].B & (0x80 >> j);
        }
    }

    for(int32_t i = 0; i < LED_NUM * 24; ++i){
        if(ledColorDataUnroll[i])LedWriteHigh();
        else LedWriteLow();
    }
    
    /*
    ledColorUpdateCnt = 0;
    ledColorPhase = 0;
    LL_GPIO_WriteOutputPort(GPIOC, LL_GPIO_ReadOutputPort(GPIOC) | (0x1));
    ledColorState = ledColorDataUnroll[0];
    if(ledColorState){
        Timer4Enable(highThTick);
    }
    else{
        Timer4Enable(lowThTick);
    } 
    */
    return 0;
}

int32_t LedInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOC)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    }
    
    LL_GPIO_InitTypeDef ledGpioInit;
    LL_GPIO_StructInit(&ledGpioInit);
    ledGpioInit.Mode = LL_GPIO_MODE_OUTPUT;
    ledGpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    ledGpioInit.Pin = LL_GPIO_PIN_0;
    ledGpioInit.Pull = LL_GPIO_PULL_DOWN;
    ledGpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOC, &ledGpioInit);

    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    uint32_t timerClk = (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1)? SysClk.PCLK1_Frequency: SysClk.PCLK1_Frequency * 2;
    lowThTick = highTlTick = timerClk / 4000000;
    highThTick = lowTlTick = timerClk / 1000000;

    Timer4Init(LedUpdateHandler);
    MyPrintf("LED init succeed!\n");
    return 0;
}

