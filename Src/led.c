#include "led.h"

LED_COLOR ledColorData[LED_NUM];
static uint8_t ledColorDataUnroll[LED_NUM * 24];

static uint8_t ledHighData = 0xf8;
static uint8_t ledLowData = 0xe0;
static uint8_t ledResetData = 0x0;

int32_t LedUpdate(){
    if(LL_SPI_IsActiveFlag_BSY(SPI2))return 1;
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
    Spi2DMATrigger(ledColorDataUnroll, LED_NUM * 24);
    return 0;
}

int32_t LedReset(){
    if(LL_SPI_IsActiveFlag_BSY(SPI2))return 1;
    Spi2DMATriggerDummy(&ledResetData, 64);
    return 0;
}

int32_t LedInit(){
    Spi2Init();
    
    UsartSetMystdioHandler(USART2);
    MyPrintf("LED init succeed!\n");
    return 0;
}

