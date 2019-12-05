#include "main.h"

void ProcessBluetoothData(char data);

int main()
{
    //Clock init
    LL_UTILS_PLLInitTypeDef pllInit;
    pllInit.PLLMul = LL_RCC_PLL_MUL_8;
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

    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);

    UsartInit();
    AudioInit();
    //MpuInit();

    uint8_t data;
    while(1) {
        SetMystdioTarget(USART2);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            SetMystdioTarget(USART1);
            MyPutchar(data);
        }
        SetMystdioTarget(USART1);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            SetMystdioTarget(USART2);
            MyPutchar(data);
            ProcessBluetoothData(data);
        }
    }
    return 0;
}

char buffer[6];
int received = 0;

void ProcessBluetoothData(char data)
{
    if(data == '@') {
        received = 1;
    }
    else if(received == 7) {
        if(data == '$') {
            color c;
            HexToColor(buffer, &c);

            // setColor(c);
        }

        received = 0;
    }
    else if(received) {
        buffer[received - 1] = data;
        received++;
    }
}
