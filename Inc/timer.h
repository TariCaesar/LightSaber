#ifndef TIMER_H
#define TIMER_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"


//Init tim2 for mpu cyclic read
int32_t Timer2Init(void (*handler)(void), uint32_t updateFrequency);

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
int32_t Timer3Init(void (*handler)(void), uint32_t updateFrequency);

static inline int32_t Timer3Enable(){
    LL_TIM_SetCounter(TIM3, 0);
    LL_TIM_EnableCounter(TIM3);
    return 0;
}

static inline int32_t Timer3Disable(){
    LL_TIM_DisableCounter(TIM3);
    return 0;
}

int32_t Timer4Init(void (*handler)(void));

static inline int32_t Timer4Enable(uint32_t tick){
    LL_TIM_SetCounter(TIM4, 0);
    LL_TIM_SetAutoReload(TIM4, tick);
    LL_TIM_EnableCounter(TIM4);
    return 0;
}

static inline int32_t Timer4Disable(){
    LL_TIM_DisableCounter(TIM4);
    return 0;
}
#endif
