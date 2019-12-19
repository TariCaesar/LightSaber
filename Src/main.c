#include "main.h"

void ProcessBluetoothData(char data);

int main()
{
    //Init SysClk
    SysClkInit();
    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);

    //Init peripheral
    UsartInit();
    MpuInit();
    AudioInit();
    LedInit();

    //for(int32_t i = 0; i < LED_NUM; ++i){
    //    ledColorData[i].G = 255;
    //    ledColorData[i].B = 0;
    //    ledColorData[i].R = 0;
    //}
    while(1) {
        //if(!AudioPlay(AUDIO_NAME_OPEN)){
        //    UsartSetMystdioHandler(USART2);
        //    MyPrintf("Start play\n");
        //}
        
        /*
        uint8_t data;
        UsartSetMystdioHandler(USART2);
        data = MyGetchar();
        if(data){
            UsartSetMystdioHandler(USART1);
            MyPutchar(data);
        }
        UsartSetMystdioHandler(USART1);
        data = MyGetchar();
        if(data){
            UsartSetMystdioHandler(USART2);
            MyPutchar(data);
            //ProcessBluetoothData(data);
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
