#ifndef SYSTEM_H
#define SYSTEM_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_utils.h"
#include "usart.h"
#include "mystdio.h"
#include "mpu6050.h"
#include "audio.h"

int32_t SysInit();
#endif
