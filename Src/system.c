#include "system.h"

static int32_t SysDelayCnt = 0;

int32_t SysClkInit(){
    //Clock init
    LL_UTILS_PLLInitTypeDef pllInit;
    pllInit.PLLMul = LL_RCC_PLL_MUL_9;
    pllInit.Prediv = LL_RCC_PREDIV_DIV_1;
    LL_UTILS_ClkInitTypeDef clkInit;
    //Set AHB the same as pll clk, 48MHz
    clkInit.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
    //Set APB1 half of the AHB clk, 24MHz
    clkInit.APB1CLKDivider = LL_RCC_APB1_DIV_2;
    //Set APB2 the same as pll clk, 48MHz
    clkInit.APB2CLKDivider = LL_RCC_APB2_DIV_1;
    //LL_PLL_ConfigSystemClock_HSI(&pllInit, &clkInit);
    LL_PLL_ConfigSystemClock_HSE(8000000u, LL_UTILS_HSEBYPASS_OFF, &pllInit, &clkInit);

    //config systick to 1ms period
    LL_RCC_ClocksTypeDef SysClk;
    LL_RCC_GetSystemClocksFreq(&SysClk);
    SysTick->LOAD = SysClk.HCLK_Frequency / 1000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(2, 0, 0));
    NVIC_EnableIRQ(SysTick_IRQn);

    //disable jtag but enable swd
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_AFIO))
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_GPIO_AF_Remap_SWJ_NOJTAG();
    return 0;
}

int32_t SysDelayMs(uint32_t tick){
    SysDelayCnt = tick;
    SysTick->VAL = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    while(SysDelayCnt)continue;
    return 0;
}

int32_t PeripheralInit(){
    UsartInit();
    PowerManageInit();
    MpuInit();
    LedInit();
    AudioInit();
    return 0;
}

int32_t PeripheralStop(){
    MpuEnterSleepMode();
    
    LL_GPIO_InitTypeDef gpioSleepMode = {
        LL_GPIO_PIN_ALL,
        LL_GPIO_MODE_ANALOG,
        LL_GPIO_SPEED_FREQ_LOW,
        LL_GPIO_OUTPUT_PUSHPULL,
        LL_GPIO_PULL_UP
    };
    LL_APB2_GRP1_EnableClock(
        LL_APB2_GRP1_PERIPH_GPIOA |
        LL_APB2_GRP1_PERIPH_GPIOB |
        LL_APB2_GRP1_PERIPH_GPIOC |
        LL_APB2_GRP1_PERIPH_GPIOD |
        LL_APB2_GRP1_PERIPH_GPIOE
    );
    LL_GPIO_Init(GPIOA, &gpioSleepMode);
    LL_GPIO_Init(GPIOB, &gpioSleepMode);
    LL_GPIO_Init(GPIOD, &gpioSleepMode);
    LL_GPIO_Init(GPIOE, &gpioSleepMode);

    gpioSleepMode.Pin = 
        LL_GPIO_PIN_0  | LL_GPIO_PIN_1  | LL_GPIO_PIN_2  | \
        LL_GPIO_PIN_3  | LL_GPIO_PIN_4  | LL_GPIO_PIN_5  | \
        LL_GPIO_PIN_6  | LL_GPIO_PIN_7  | LL_GPIO_PIN_9  | \
        LL_GPIO_PIN_10  | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | \
        LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOC, &gpioSleepMode);
    LL_APB2_GRP1_DisableClock(
        LL_APB2_GRP1_PERIPH_GPIOA |
        LL_APB2_GRP1_PERIPH_GPIOB |
        LL_APB2_GRP1_PERIPH_GPIOD |
        LL_APB2_GRP1_PERIPH_GPIOE
    );
    return 0;
}

int32_t InterruptDisable(){
    NVIC_DisableIRQ(I2C2_EV_IRQn);
    NVIC_ClearPendingIRQ(I2C2_EV_IRQn);
    NVIC_DisableIRQ(DMA1_Channel6_IRQn);
    NVIC_ClearPendingIRQ(DMA1_Channel6_IRQn);
    NVIC_DisableIRQ(TIM2_IRQn);
    NVIC_ClearPendingIRQ(TIM2_IRQn);
    NVIC_DisableIRQ(TIM3_IRQn);
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    NVIC_DisableIRQ(DMA1_Channel2_IRQn);
    NVIC_ClearPendingIRQ(DMA1_Channel2_IRQn);
    NVIC_DisableIRQ(DMA1_Channel3_IRQn);
    NVIC_ClearPendingIRQ(DMA1_Channel3_IRQn);
    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(USART1_IRQn);
    NVIC_ClearPendingIRQ(USART1_IRQn);
    NVIC_DisableIRQ(USART2_IRQn);
    NVIC_ClearPendingIRQ(USART2_IRQn);
    return 0;
}

void SysTick_Handler(){
    SysDelayCnt -= 1;
    if(!SysDelayCnt)SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    return;
}

