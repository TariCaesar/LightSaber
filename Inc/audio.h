#ifndef AUDIO_H
#define AUDIO_H

//#ifdef STM32F103xE

#include "stm32f1xx.h"
#include "mystdio.h"
#include "extern_flash.h"
#include "dac.h"

typedef enum _AUDIO_NAME {
    AUDIO_NAME_OPEN = 0,
    AUDIO_NAME_HUM = 1,
    AUDIO_NAME_SWING = 2
} AUDIO_NAME;

int32_t AudioInit();

//data size in bytes
int32_t AudioStore(int16_t* addrSrc, int32_t size, int32_t addrDst);
int32_t AudioPlay(int16_t addr);

//#endif
#endif
