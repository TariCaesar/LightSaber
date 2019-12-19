#include "system.h"

static int32_t SysDelayCnt = 0;

int32_t SysClkInit(){
    //Clock init
    LL_UTILS_PLLInitTypeDef pllInit;
    pllInit.PLLMul = LL_RCC_PLL_MUL_9;
    pllInit.Prediv = LL_RCC_PREDIV_DIV_1;
    LL_UTILS_ClkInitTypeDef clkInit;
    //Set AHB the same as pll clk, 72MHz
    clkInit.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
    //Set APB1 half of the AHB clk, 36MHz
    clkInit.APB1CLKDivider = LL_RCC_APB1_DIV_2;
    //Set APB2 the same as pll clk, 72MHz
    clkInit.APB2CLKDivider = LL_RCC_APB2_DIV_1;
    LL_PLL_ConfigSystemClock_HSI(&pllInit, &clkInit);
    //LL_PLL_ConfigSystemClock_HSE(8000000u, LL_UTILS_HSEBYPASS_OFF, &pllInit, &clkInit);

    //config systick to 1ms period
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    SysTick->LOAD = SysClk.HCLK_Frequency / 1000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(2, 0, 0));
    NVIC_EnableIRQ(SysTick_IRQn);

    return 0;
}

int32_t SysDelayMs(uint32_t tick){
    SysDelayCnt = tick;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    while(SysDelayCnt)continue;
    return 0;
}

void SysTick_Handler(){
    SysDelayCnt -= 1;
    //MyPrintf("%d\n", SysDelayCnt);
    if(!SysDelayCnt)SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    return;
}

