#include "audio.h"

int32_t audioStartSector[] = {1, 783, 1018};
//audio length in Sectors
int32_t audioLen[] = {782, 235, 196};
int16_t audioHeadBuffer[sizeof(audioStartSector) / 4][EXTERN_FLASH_SECTOR_SIZE / 2];

int32_t AudioInit()
{
    if(ExternFlashInit()){
        SetMystdioTarget(USART2);
        MyPrintf("Audio init fail due to ExternalFlash check fail!\n");
        return 1;
    }
    else{
        MyPrintf("Start preload audio data!\n");
        for(int32_t i = 0; i < sizeof(audioStartSector) / 4; ++i){
            
        }
        MyPrintf("Audio init success!\n");
    }
}

int32_t AudioStore(int16_t* addrSrc, int32_t Size, int32_t addrDst)
{
    if(!ExternFlashInit())return 0;
    
}

