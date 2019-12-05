#ifndef USART_H
#define USART_H

#include "stm32f1xx.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_usart.h"

#include "mystdio.h"

//due to our full judgement alogrithm, buffer size is 1 less than them
#define USART_RX_BUFFER_SIZE 128
#define USART_TX_BUFFER_SIZE 128
#define USART_TX_DMA_THRESHOLD 4

int32_t UsartInit();
int32_t UsartRxBufferIsEmpty(USART_TypeDef* usartTarget);
int32_t UsartTxBufferIsFull(USART_TypeDef* usartTarget);
uint32_t UsartReceiveData(uint8_t* addr_dst, uint32_t size, USART_TypeDef* usartTarget);
uint32_t UsartSendData(uint8_t* addr_src, uint32_t size, USART_TypeDef* usartTarget);
#endif
