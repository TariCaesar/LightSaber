#ifndef HELPER_H
#define HELPER_H

#include "stm32f1xx.h"
#include "led.h"

void HexToColor(const uint8_t* hexString, LED_COLOR* result);
void ProcessBluetoothData(uint8_t data);

#endif
