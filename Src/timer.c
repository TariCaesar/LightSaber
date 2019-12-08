#include "timer.h"

static void (*timer2Handler)(void) = 0;

int32_t Timer2Init(){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_InitTypeDef timer2Init;
    LL_TIM_StructInit(&timer2Init);
    //config tim2 frequency to APB1_CLK / 10000;
    timer2Init.Prescaler = 2499u;
    timer2Init.ClockDivision = LL_TIM_CLOCKDIVISION_DIV4;
    timer2Init.CounterMode = LL_TIM_COUNTERMODE_UP;
    timer2Init.RepetitionCounter = LL_TIM_ONEPULSEMODE_REPETITIVE;

    //set timer trigger frequency to 100Hz
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    if(LL_RCC_GetAPB1Prescaler() == 1)SysClk.PCLK1_Frequency *= 2;
    timer2Init.Autoreload = SysClk.PCLK1_Frequency / 1000000;
    
    LL_TIM_Init(TIM2, &timer2Init);

    LL_TIM_EnableIT_UPDATE(TIM2);

    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM2_IRQn);

    LL_TIM_SetCounter(TIM2, 0);
    
    //Init but don't enable counter
    return 0;
}

int32_t Timer2Enable(void (*handler)(void)){
    timer2Handler = handler;
    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);
    return 0;
}

int32_t Timer2Disable(){
    LL_TIM_DisableCounter(TIM2);
    return 0;
}

void TIM2_IRQHandler(){
    if(timer2Handler == 0)return;
    else{
        timer2Handler();
        return;
    }
}
