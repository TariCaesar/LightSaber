#ifndef TIMER_H
#define TIMER_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_tim.h"

//Init tim2 for mpu cyclic read
int32_t Timer2Init();
int32_t Timer2Enable(void (*timer2Handler)(void));
int32_t Timer2Disable();

#endif
