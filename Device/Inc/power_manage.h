#ifndef POWER_MANAGE_H
#define POWER_MANAGE_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "system.h"
#include "mystdio.h"
#include "usart.h"

extern volatile int32_t systemNeedShutdown;
extern volatile int32_t shutdownAudioPlayed;

void ShutdownAudioCallbackHandler();

int32_t PowerManageInit();

int32_t PowerBootManage();

int32_t EnterStopMode();

#endif
