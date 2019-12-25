#ifndef LED_H
#define LED_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_spi.h"
#include "mystdio.h"
#include "usart.h"
#include "timer.h"

#define LED_NUM 20
#define LED_RESET_DATA_LENGTH 64

typedef struct{
    uint8_t R;
    uint8_t G;
    uint8_t B;
}LED_COLOR;

extern LED_COLOR ledColorData[LED_NUM];

int32_t LedInit();

int32_t LedUpdate();

int32_t LedIsUpdating();

#endif
