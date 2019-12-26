#ifndef AUDIO_H
#define AUDIO_H


#include "stm32f1xx.h"
#include "mystdio.h"
#include "usart.h"
#include "extern_flash.h"
#include "dac.h"

typedef enum{
    AUDIO_NAME_OPEN = 0,
    AUDIO_NAME_HUM = 1,
    AUDIO_NAME_SWING = 2,
    AUDIO_NAME_CLOSE = 3
}AUDIO_NAME;

int32_t AudioInit();

int32_t AudioPlay(AUDIO_NAME audioName, void (*callbackHandler)(void));

#endif
