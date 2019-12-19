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
    //use dma for led transfer, configure PB15(SPI2 MOSI) into af output
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }
    
    LL_GPIO_InitTypeDef ledGpioInit;
    LL_GPIO_StructInit(&ledGpioInit);
    ledGpioInit.Pin = LL_GPIO_PIN_15;
    ledGpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    ledGpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    ledGpioInit.Pull = LL_GPIO_PULL_UP;
    ledGpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOB, &ledGpioInit);

    
    MyPrintf("LED init succeed!\n");
    return 0;
}

