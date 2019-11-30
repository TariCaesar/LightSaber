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
    LL_PLL_ConfigSystemClock_HSE(8000000u, LL_UTILS_HSEBYPASS_OFF, &pllInit, &clkInit);

    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);

    //after init, print clk information and usart init complete
    char UsartInitSuccessString[] = "Usart init Success!\n";
    while(UsartInit())
        continue;
    UsartSendData((uint8_t*)UsartInitSuccessString, sizeof(UsartInitSuccessString), USART1);

    char SpiInitSuccessString[] = "Spi init Success!\n";
    char SpiInitFailString[] = "Spi init Fail!\n";
    if(!SpiInit())
        UsartSendData((uint8_t*)SpiInitSuccessString, sizeof(SpiInitSuccessString), USART1);
    else
        UsartSendData((uint8_t*)SpiInitFailString, sizeof(SpiInitFailString), USART1);

    char ExtFlashCheckPassString[] = "External Flash self check pass!\n";
    char ExtFlashCheckFailString[] = "External Flash self check fail!\n";
    if(!ExternFlashInit())
        UsartSendData((uint8_t*)ExtFlashCheckPassString, sizeof(ExtFlashCheckPassString), USART1);
    else
        UsartSendData((uint8_t*)ExtFlashCheckFailString, sizeof(ExtFlashCheckFailString), USART1);

    char MpuCheckPassString[] = "Mpu self check pass!\n";
    char MpuCheckFailString[] = "Mpu self check fail!\n";
    if(!MpuInit())
        UsartSendData((uint8_t*)MpuCheckPassString, sizeof(MpuCheckPassString), USART1);
    else
        UsartSendData((uint8_t*)MpuCheckFailString, sizeof(MpuCheckFailString), USART1);

    uint8_t data;
    while(1) {
        if(UsartReceiveData(&data, 1, USART1))
            UsartSendData(&data, 1, USART2);
        if(UsartReceiveData(&data, 1, USART2)) {
            UsartSendData(&data, 1, USART1);
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
