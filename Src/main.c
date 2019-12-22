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
    LedInit();
    AudioInit();
    PowerManageInit();
    
    DeviceEnable();

    if(!AudioPlay(AUDIO_NAME_OPEN)){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Play open audio\n");
    }
    while(1) {
        if(deviceIsSwing){
            if(!AudioPlay(AUDIO_NAME_SWING)){
                UsartSetMystdioHandler(USART2);
                MyPrintf("Play swing audio\n");
            }
        }
        else{
            if(!AudioPlay(AUDIO_NAME_HUM)){
                UsartSetMystdioHandler(USART2);
                MyPrintf("Play hum audio\n");
            }
        }
        
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
