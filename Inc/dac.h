#ifndef DAC_H 
#define DAC_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_dac.h"
#include "mystdio.h"
#include "usart.h"
#include "timer.h"

#define DAC_MAX 128

int32_t DacInit();

//this function put data into dac output reg in 44100Hz frequency
//provide a handler access to handle process after play finish
uint32_t DacAudioPlay(int16_t* data, uint32_t size, void(*handler)(void));


#endif
