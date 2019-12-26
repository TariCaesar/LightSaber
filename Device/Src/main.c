#include "main.h"

int main()
{
    //Init SysClk
    SysClkInit();
    //Set priorityGroup to 2 for all the system
    NVIC_SetPriorityGrouping(2);

    //Init peripheral
    UsartInit();
    PowerManageInit();
    MpuInit();
    LedInit();
    AudioInit();
    
    PowerBootManage();

    for(int32_t i = 0; i < LED_NUM; ++i){
        ledColorData[i].R = 0xff;
        ledColorData[i].G = 0x00;
        ledColorData[i].B = 0x00;
    }
    if(!AudioPlay(AUDIO_NAME_OPEN, 0)){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Play open audio\n");
    }

    while(1) {
        if(shutdownAudioPlayed){
            EnterStopMode();
        }
        else if(systemNeedShutdown){
            if(!AudioPlay(AUDIO_NAME_CLOSE, ShutdownAudioCallbackHandler)){
                UsartSetMystdioHandler(USART2);
                MyPrintf("Play shutdown audio.\n");
            }
        }
        else{
            if(deviceIsSwing){
                if(!AudioPlay(AUDIO_NAME_SWING, 0)){
                    UsartSetMystdioHandler(USART2);
                    MyPrintf("Play swing audio\n");
                }
            }
            else{
                if(!AudioPlay(AUDIO_NAME_HUM, 0)){
                    UsartSetMystdioHandler(USART2);
                    MyPrintf("Play hum audio\n");
                }
            }
        }
       
        //deal with 
        uint8_t data;
        UsartSetMystdioHandler(USART1);
        data = MyGetchar();
        if(data){
            ProcessBluetoothData(data);
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
            ProcessBluetoothData(data);
        }
        */
    }
    return 0;
}

