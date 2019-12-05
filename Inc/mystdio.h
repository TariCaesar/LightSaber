#ifndef MYSTDIO_H
#define MYSTDIO_H

#include "usart.h"
#include <stdio.h>

int32_t SetMystdioTarget(USART_TypeDef* usartTarget);

int32_t MyPrintf(const char*, ...);
int32_t MyScanf(const char*, ...);

uint8_t MyGetchar();
uint8_t MyPutchar(uint8_t ch);

int32_t MystdoutBufferIsFull();
int32_t MystdinBufferIsEmpty();

#endif
