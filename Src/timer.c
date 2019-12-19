#include "timer.h"

static void (*timer2Handler)(void) = 0;
static void (*timer3Handler)(void) = 0;
static void (*timer4Handler)(void) = 0;

int32_t Timer2Init(void (*handler)(void), uint32_t updateFrequency){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_DeInit(TIM2);
    LL_TIM_InitTypeDef timer2Init;
    LL_TIM_StructInit(&timer2Init);
    //config tim2 clk frequency to APB1_CLK / 10000;
    timer2Init.Prescaler = 9999u;
    timer2Init.CounterMode = LL_TIM_COUNTERMODE_UP;

    //set timer trigger frequency to 100Hz
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    uint32_t timerClk = (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1)? SysClk.PCLK1_Frequency / 10000: SysClk.PCLK1_Frequency * 2 / 10000;
    if(updateFrequency > timerClk)return 1;
    timer2Init.Autoreload = timerClk / updateFrequency - 1;
    
    LL_TIM_Init(TIM2, &timer2Init);

    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM2_IRQn);
    timer2Handler = handler;

    return 0;
}

void TIM2_IRQHandler(){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)){
        LL_TIM_ClearFlag_UPDATE(TIM2);
        if(timer2Handler != 0){
            timer2Handler();
        }
    }
    return;
}

int32_t Timer3Init(void (*handler)(void), uint32_t updateFrequency){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
    LL_TIM_DeInit(TIM3);
    LL_TIM_InitTypeDef timer3Init;
    LL_TIM_StructInit(&timer3Init);
    //config tim3 clk frequency to APB1_CLK;
    timer3Init.Prescaler = 0;
    timer3Init.CounterMode = LL_TIM_COUNTERMODE_UP;

    //set timer trigger frequency to given frequency
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    uint32_t timerClk = (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1)? SysClk.PCLK1_Frequency: SysClk.PCLK1_Frequency * 2;
    timer3Init.Autoreload = timerClk / updateFrequency - 1;
    
    LL_TIM_Init(TIM3, &timer3Init);

    LL_TIM_ClearFlag_UPDATE(TIM3);
    LL_TIM_EnableIT_UPDATE(TIM3);
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM3_IRQn);

    timer3Handler = handler;
    return 0;
}

void TIM3_IRQHandler(){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM3)){
        LL_TIM_ClearFlag_UPDATE(TIM3);
        if(timer3Handler != 0){
            timer3Handler();
        }
    }
    return;
}

int32_t Timer4Init(void (*handler)(void)){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
    LL_TIM_DeInit(TIM4);
    LL_TIM_InitTypeDef timer4Init;
    LL_TIM_StructInit(&timer4Init);
    //config tim4 clk frequency to APB1_CLK;
    timer4Init.Prescaler = 0;
    timer4Init.CounterMode = LL_TIM_COUNTERMODE_UP;

    LL_TIM_Init(TIM4, &timer4Init);

    LL_TIM_ClearFlag_UPDATE(TIM4);
    LL_TIM_EnableIT_UPDATE(TIM4);
    NVIC_SetPriority(TIM4_IRQn, NVIC_EncodePriority(2, 1, 0));
    NVIC_EnableIRQ(TIM4_IRQn);
    timer4Handler = handler;

    return 0;
}

void TIM4_IRQHandler(){
    LL_TIM_DisableCounter(TIM4);
    if(timer4Handler != 0){
        timer4Handler();
    }
    LL_TIM_ClearFlag_UPDATE(TIM4);
    return;
}
