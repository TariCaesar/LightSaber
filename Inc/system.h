#ifndef SYSTEM_H
#define SYSTEM_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_utils.h"
#include "mystdio.h"
#include "helper.h"
#include "usart.h"
#include "mystdio.h"
#include "mpu6050.h"
#include "audio.h"
#include "led.h"
#include "power_manage.h"

int32_t SysClkInit();
int32_t SysDelayMs(uint32_t tick);
int32_t PeripheralInit();
int32_t PeripheralStop();
int32_t InterruptDisable();

#endif
