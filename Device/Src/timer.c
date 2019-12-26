#include "timer.h"

static void (*timer1DmaCallbackHandler)(void) = 0;
static void (*timer2IntHandler)(void) = 0;
static void (*timer3IntHandler)(void) = 0;
static void (*timer4IntHandler)(void) = 0;

static inline int32_t Timer1Enable(){
    LL_TIM_SetCounter(TIM1, 0);
    LL_TIM_EnableCounter(TIM1);
    return 0;
}

static inline int32_t Timer1Disable(){
    LL_TIM_DisableCounter(TIM1);
    return 0;
}

int32_t Timer1Init(){
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    LL_TIM_DeInit(TIM1);
    LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_InitTypeDef timer1Init;
    LL_TIM_StructInit(&timer1Init);
    timer1Init.Prescaler = 0u;
    timer1Init.CounterMode = LL_TIM_COUNTERMODE_UP;

    //set timer trigger frequency to 100Hz
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    uint32_t timerClk = (LL_RCC_GetAPB2Prescaler() == LL_RCC_APB2_DIV_1)? SysClk.PCLK2_Frequency: SysClk.PCLK2_Frequency * 2;
    timer1Init.Autoreload = timerClk / 800000;
    
    LL_TIM_Init(TIM1, &timer1Init);

    LL_TIM_OC_InitTypeDef timer1OcInit;
    LL_TIM_OC_StructInit(&timer1OcInit);
    timer1OcInit.CompareValue = 0u;
    timer1OcInit.OCMode = LL_TIM_OCMODE_PWM1;
    timer1OcInit.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    timer1OcInit.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    timer1OcInit.OCNState = LL_TIM_OCSTATE_ENABLE;
    timer1OcInit.OCState = LL_TIM_OCSTATE_DISABLE;
    LL_TIM_OC_Init(TIM1, LL_TIM_CHANNEL_CH3, &timer1OcInit);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_OC_SetDeadTime(TIM1, 0);
    
    if(!LL_AHB1_GRP1_IsEnabledClock(LL_AHB1_GRP1_PERIPH_DMA1)) {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    }

    LL_DMA_ClearFlag_TC6(DMA1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_6);
    NVIC_SetPriority(DMA1_Channel6_IRQn, NVIC_EncodePriority(2, 2, 0));
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
    LL_DMA_ConfigTransfer(
            DMA1, 
            LL_DMA_CHANNEL_6,
            LL_DMA_DIRECTION_MEMORY_TO_PERIPH |
            LL_DMA_PRIORITY_VERYHIGH |
            LL_DMA_MODE_NORMAL |
            LL_DMA_PERIPH_NOINCREMENT |
            LL_DMA_MEMORY_INCREMENT |
            LL_DMA_PDATAALIGN_HALFWORD |
            LL_DMA_MDATAALIGN_HALFWORD
    );
    return 0;
}

int32_t Timer1Trigger(uint16_t* dataSrc, uint32_t size, void (*dmaCallbackHandler)(void)){
    timer1DmaCallbackHandler = dmaCallbackHandler;
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_6, size - 1);
    LL_DMA_ConfigAddresses(
        DMA1,
        LL_DMA_CHANNEL_6,
        (uint32_t)(dataSrc + 1),
        (uint32_t)(&(TIM1->CCR3)),
        LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_6)
    );
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_6);
    LL_TIM_EnableDMAReq_CC3(TIM1);
    LL_TIM_OC_SetCompareCH3(TIM1, dataSrc[0]);
    Timer1Enable();
    return 0;
}

void DMA1_Channel6_IRQHandler(){
    LL_DMA_ClearFlag_TC6(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_6);
    LL_TIM_DisableDMAReq_CC3(TIM1);
    Timer1Disable();
    if(timer1DmaCallbackHandler != 0)timer1DmaCallbackHandler();
    return;
}

int32_t Timer2Init(void (*intHandler)(void), uint32_t frequency){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_DeInit(TIM2);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
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
    LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
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

int32_t Timer4Init(void (*intHandler)(void), uint32_t frequency){
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
    LL_TIM_DeInit(TIM4);
    LL_TIM_SetClockSource(TIM4, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_InitTypeDef timer4Init;
    LL_TIM_StructInit(&timer4Init);
    //config tim4 clk frequency to APB1_CLK;
    timer4Init.Prescaler = 9999u;
    timer4Init.CounterMode = LL_TIM_COUNTERMODE_UP;

    //set timer trigger frequency to given frequency
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    uint32_t timerClk = (LL_RCC_GetAPB1Prescaler() == LL_RCC_APB1_DIV_1)? SysClk.PCLK1_Frequency / 10000: SysClk.PCLK1_Frequency * 2 / 10000;
    timer4Init.Autoreload = timerClk / frequency - 1;
    
    LL_TIM_Init(TIM4, &timer4Init);

    LL_TIM_ClearFlag_UPDATE(TIM4);
    LL_TIM_EnableIT_UPDATE(TIM4);
    NVIC_SetPriority(TIM4_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(TIM4_IRQn);

    timer4IntHandler = intHandler; 
    return 0;
}

void TIM4_IRQHandler(){
    if(LL_TIM_IsActiveFlag_UPDATE(TIM4)){
        LL_TIM_ClearFlag_UPDATE(TIM4);
        if(timer4IntHandler != 0){
            timer4IntHandler();
        }
    }
    return;
}
