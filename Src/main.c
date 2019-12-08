#include "main.h"

void ProcessBluetoothData(char data);

int main()
{
    //Init SysClk
    SysClkInit();
    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);
    //Init SysDelay
    SysDelayInit();
    //Init peripheral
    UsartInit();
    //MpuInit();
    AudioInit();

    while(1) {
        /*
        uint8_t data;
        UsartSetMystdioHandler(USART2);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            UsartSetMystdioHandler(USART1);
            MyPutchar(data);
        }
        UsartSetMystdioHandler(USART1);
        if(!MystdinBufferIsEmpty()){
            data = MyGetchar();
            UsartSetMystdioHandler(USART2);
            MyPutchar(data);
            ProcessBluetoothData(data);
        }
        */
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
