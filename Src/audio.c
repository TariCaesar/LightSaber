#include "audio.h"

static int32_t audioStartSector[] = {1, 50, 65};
//audio length in Sectors
static int32_t audioLen[] = {49, 15, 13};
static int16_t audioHeadBuffer[sizeof(audioStartSector) / sizeof(int32_t)][EXTERN_FLASH_SECTOR_SIZE / sizeof(int16_t)];
//define ping pong buffer
static int16_t audioBuffer[2][EXTERN_FLASH_SECTOR_SIZE / sizeof(int16_t)];
static int32_t audioBufferPingOrPong = 0;
static int32_t audioBufferReadCnt = 0;
static int32_t audioBufferReadNum = 0;
static int32_t audioBufferAddrReadNext = 0;

static int32_t audioIsPlaying = 0;

static void audioCallbackHandler(){
    if(audioBufferReadCnt < audioBufferReadNum){
        DacAudioPlay((int16_t*)(audioBuffer[audioBufferPingOrPong]), EXTERN_FLASH_SECTOR_SIZE / sizeof(int16_t), audioCallbackHandler);
        audioBufferPingOrPong = (audioBufferPingOrPong)? 0: 1;
        audioBufferAddrReadNext += EXTERN_FLASH_SECTOR_SIZE;

        FlashFastRead(audioBufferAddrReadNext, (uint8_t*)(audioBuffer[audioBufferPingOrPong]), EXTERN_FLASH_SECTOR_SIZE, 0);
        audioBufferReadCnt += 1;
    }
    else if(audioBufferReadCnt == audioBufferReadNum){
        DacAudioPlay((int16_t*)(audioBuffer[audioBufferPingOrPong]), EXTERN_FLASH_SECTOR_SIZE / 2, audioCallbackHandler);
        audioBufferReadCnt += 1;
    }
    else{
        audioIsPlaying = 0;
    } 
    return;
} 

int32_t AudioInit(){
    if(DacInit()){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Audio initializaion fail due to dac initialization fail.\n");
        return 1;
    }
    if(ExternFlashInit()){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Audio initializaion fail due to external flash check fail.\n");
        return 1;
    }

    UsartSetMystdioHandler(USART2);
    MyPrintf("Audio initializaion succeed!\n");
    MyPrintf("Start preload audio data.\n");
    for(int32_t i = 0; i < sizeof(audioStartSector) / sizeof(int32_t); ++i){
        FlashRead(
            audioStartSector[i] * EXTERN_FLASH_SECTOR_SIZE,
            (uint8_t*)(audioHeadBuffer[i]),
            EXTERN_FLASH_SECTOR_SIZE
        );
    }

    MyPrintf("Audio data preload finish.\n");
    return 0;
}

int32_t AudioPlay(AUDIO_NAME audioName){
    if(audioIsPlaying)return 1;
    audioBufferReadCnt = 1;
    audioBufferReadNum = audioLen[audioName];
    audioBufferPingOrPong = 0;
    audioBufferAddrReadNext = (audioStartSector[audioName] + 1) * EXTERN_FLASH_SECTOR_SIZE;
    FlashFastRead(audioBufferAddrReadNext, (uint8_t*)(audioBuffer[audioBufferPingOrPong]), EXTERN_FLASH_SECTOR_SIZE, 0);
    audioBufferReadCnt += 1;

    DacAudioPlay((int16_t*)(audioHeadBuffer[audioName]), EXTERN_FLASH_SECTOR_SIZE / sizeof(int16_t), audioCallbackHandler);
    audioIsPlaying = 1;
    return 0;
}
