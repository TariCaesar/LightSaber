#include "timer.h"

static void (*timer2IntHandler)(void) = 0;
static void (*timer3IntHandler)(void) = 0;

int32_t Timer2Init(void (*intHandler)(void), uint32_t frequency){
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
    if(frequency > timerClk)return 1;
    timer2Init.Autoreload = timerClk / frequency - 1;
    
    LL_TIM_Init(TIM2, &timer2Init);

    LL_TIM_ClearFlag_UPDATE(TIM2);
    LL_TIM_EnableIT_UPDATE(TIM2);
    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM2_IRQn);
    timer2IntHandler = intHandler;

    return 0;
}

void TIM2_IRQHandler(){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM2)){
        LL_TIM_ClearFlag_UPDATE(TIM2);
        if(timer2IntHandler != 0){
            timer2IntHandler();
        }
    }
    return;
}

int32_t Timer3Init(void (*intHandler)(void), uint32_t frequency){
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
    timer3Init.Autoreload = timerClk / frequency - 1;
    
    LL_TIM_Init(TIM3, &timer3Init);

    LL_TIM_ClearFlag_UPDATE(TIM3);
    LL_TIM_EnableIT_UPDATE(TIM3);
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM3_IRQn);

    timer3IntHandler = intHandler;
    return 0;
}

void TIM3_IRQHandler(){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM3)){
        LL_TIM_ClearFlag_UPDATE(TIM3);
        if(timer3IntHandler != 0){
            timer3IntHandler();
        }
    }
    return;
}
