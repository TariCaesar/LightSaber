#include "main.h"

int main(){
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
    LL_PLL_ConfigSystemClock_HSE(8000000u, LL_UTILS_HSEBYPASS_OFF, &pllInit, &clkInit);

    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);
    //Enable dma clock
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    //after init, print clk information and usart init complete
    while(UsartInit());
    LL_RCC_ClocksTypeDef sysClk;
    LL_RCC_GetSystemClocksFreq(&sysClk);
    printf("System Clock frequency: %d\n", (int)sysClk.SYSCLK_Frequency);
    printf("AHB Clock frequency: %d\n", (int)sysClk.HCLK_Frequency);
    printf("APB1 Clock frequency: %d\n", (int)sysClk.PCLK1_Frequency);
    printf("APB2 Clock frequency: %d\n", (int)sysClk.PCLK2_Frequency);
    printf("Usart init Complete!\n");

    if(!SpiInit())printf("Spi init complete!\n");
    else printf("Spi Init Fail");

    if(!ExternFlashInit())printf("Extern flash self-check pass!\n");
    else printf("Extern flash self-check fail!");

    while(1){
    }
    return 0;
}
