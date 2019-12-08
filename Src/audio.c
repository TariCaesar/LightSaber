#include "audio.h"

int32_t audioStartSector[] = {1, 50, 65};
//audio length in Sectors
int32_t audioLen[] = {49, 15, 13};
int16_t audioHeadBuffer[sizeof(audioStartSector) / 4][EXTERN_FLASH_SECTOR_SIZE / 2];

int32_t AudioInit()
{
    if(ExternFlashInit()){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Audio initializaion fail due to external flash check fail.\n");
        return 1;
    }
    else{
        MyPrintf("Audio initializaion succeed!\n");
        UsartSetMystdioHandler(USART2);
        MyPrintf("Start preload audio data.\n");
        for(int32_t i = 0; i < sizeof(audioStartSector) / 4; ++i){
            FlashRead(
                audioStartSector[i] * EXTERN_FLASH_SECTOR_SIZE,
                (uint8_t*)(audioHeadBuffer[i]),
                EXTERN_FLASH_SECTOR_SIZE
            );
        }
        MyPrintf("Audio data preload finish.\n");
    }
}

int32_t AudioStore(int16_t* addrSrc, int32_t Size, int32_t addrDst)
{
    if(!ExternFlashInit())return 0;
    
}

