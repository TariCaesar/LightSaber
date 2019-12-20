#ifndef POWER_MANAGE_H
#define POWER_MANAGE_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_bus.h"
#include "mystdio.h"
#include "usart.h"


int32_t PowerManageInit();

int32_t DeviceEnable();

int32_t DeviceDisable();

#endif
