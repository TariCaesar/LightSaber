#ifndef DAC_H 
#define DAC_H

#ifdef STM32F103xE
#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_dac.h"
#include "mystdio.h"
#include "usart.h"

#define AUDIO_MAX 128

int32_t DacInit();

#endif

#endif
