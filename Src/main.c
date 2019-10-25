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

    //wait until usartinit complete
    while(UsartInit());
    char usartInitCompleteString[] = "Usart init complete!\n";
    UsartSendData((uint8_t*)usartInitCompleteString, sizeof(usartInitCompleteString));
    
    while(SpiInit());
    char spiInitCompleteString[] = "Spi init complete!\n";
    UsartSendData((uint8_t*)spiInitCompleteString, sizeof(spiInitCompleteString));

    while(1){
    }
    return 0;
}
