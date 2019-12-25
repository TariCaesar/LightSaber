#include "led.h"

LED_COLOR ledColorData[LED_NUM];
static uint16_t ledColorDataUnroll[LED_NUM * 24];

const uint16_t ledHighData = 66;
const uint16_t ledLowData = 33;
const uint16_t ledResetData[LED_RESET_DATA_LENGTH] = {0};

static int32_t ledIsUpdating = 0;

static void LedUpdateCallbackHandler(){
    Timer1DisableOutput();
    ledIsUpdating = 0;
    return;
}

static void LedResetCallbackHandler(){
    Timer1EnableOutput();
    Timer1Trigger(ledColorDataUnroll, LED_NUM * 24, LedUpdateCallbackHandler);
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
    Timer1DisableOutput();
    Timer1Trigger(ledResetData, LED_RESET_DATA_LENGTH, LedResetCallbackHandler);
    return 0;
}

int32_t LedIsUpdating(){
    return ledIsUpdating;
}

int32_t LedInit(){
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    LL_GPIO_InitTypeDef ledGpioInit;
    LL_GPIO_StructInit(&ledGpioInit);
    ledGpioInit.Pin = LL_GPIO_PIN_15;
    ledGpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    ledGpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    ledGpioInit.Pull = LL_GPIO_PULL_UP;
    ledGpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    LL_GPIO_Init(GPIOB, &ledGpioInit);

    Timer1Init();

    ledIsUpdating = 0;

    UsartSetMystdioHandler(USART2);
    MyPrintf("LED init succeed!\n");
    return 0;
}

