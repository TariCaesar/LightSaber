#ifndef TIMER_H
#define TIMER_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_dma.h"

//Init tim1 for led data output 
int32_t Timer1Init();

int32_t Timer1Trigger(uint16_t* dataSrc, uint32_t size, void (*dmaCallbackHandler)(void));

static inline int32_t Timer1EnableOutput(){
    LL_TIM_EnableAllOutputs(TIM1);
    return 0;
}

static inline int32_t Timer1DisableOutput(){
    LL_TIM_DisableAllOutputs(TIM1);
    return 0;
}

//Init tim2 for mpu cyclic read
int32_t Timer2Init(void (*intHandler)(void), uint32_t frequency);

static inline int32_t Timer2Enable(){
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    return 0;
}

static inline int32_t Timer2Disable(){
    LL_TIM_DisableCounter(TIM2);
    return 0;
}

//Init tim3 for dac data output 
int32_t Timer3Init(void (*intHandler)(void), uint32_t frequency);

static inline int32_t Timer3Enable(){
    LL_TIM_SetCounter(TIM3, 0);
    LL_TIM_EnableCounter(TIM3);
    return 0;
}

static inline int32_t Timer3Disable(){
    LL_TIM_DisableCounter(TIM3);
    return 0;
}

#endif
