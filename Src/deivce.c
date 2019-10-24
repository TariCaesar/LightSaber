#include "device.h"

int DeviceInit(){
    LL_RCC_HSE_Enable();
    //wait until HSE Ready
    while(!LL_RCC_HSE_IsReady());
    //change SysClk source to HSE
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSE);

    //set AHB clock the same as sysclk
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    //set APB1, APB2 clock the same as AHB clk
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);
    return 0;
}