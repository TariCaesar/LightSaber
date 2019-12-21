#include "led.h"

LED_COLOR ledColorData[LED_NUM];
static uint8_t ledColorDataUnroll[LED_NUM * 24];

static uint8_t ledHighData = 0xfc;
static uint8_t ledLowData = 0xc0;
static uint8_t ledResetData = 0x00;

static int32_t ledIsUpdating = 0;

static void LedResetCallbackHandler(){
    ledIsUpdating = 0;
    return;
}

static int32_t LedReset(){
    Spi2WriteDummyDma(&ledResetData, 128, LedResetCallbackHandler);
    return 0;
}

static void LedUpdataCallbackHandler(){
    //LedReset();
    return;
}

int32_t LedUpdate(){
    if(ledIsUpdating)return 1;
    ledIsUpdating = 1;
    for(int32_t i = 0; i < LED_NUM; ++i){
        for(int32_t j = 0; j < 8; ++j){
            ledColorDataUnroll[i * 24 + j] =
                ledColorData[i].G & (0x80 >> j)?
                ledHighData: ledLowData;
            ledColorDataUnroll[i * 24 + j + 8] =
                ledColorData[i].R & (0x80 >> j)?
                ledHighData: ledLowData;
            ledColorDataUnroll[i * 24 + j + 16] =
                ledColorData[i].B & (0x80 >> j)?
                ledHighData: ledLowData;
        }
    }
    Spi2WriteDma(ledColorDataUnroll, LED_NUM * 24, LedUpdataCallbackHandler);
    return 0;
}

int32_t LedInit(){
    Spi2Init();
    
    ledIsUpdating = 0;

    UsartSetMystdioHandler(USART2);
    MyPrintf("LED init succeed!\n");
    return 0;
}

