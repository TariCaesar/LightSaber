#ifndef SYSTEM_H
#define SYSTEM_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_utils.h"


int32_t SysClkInit();
int32_t SysDelayMs(uint32_t tick);
int32_t SysDelayInit();
#endif
