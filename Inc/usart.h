#ifndef USART_H
#define USART_H

#include "stm32f103xb.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_dma.h"

//due to our full judgement alogrithm, buffer size is 1 less than them
#define USART_RX_BUFFER_SIZE 256
#define USART_TX_BUFFER_SIZE 256
#define USART_TX_DMA_THRESHOLD 5

typedef struct{
    uint8_t data[USART_RX_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
}UsartRxBuffer;

typedef struct{
    uint8_t data[USART_TX_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
    uint32_t tailNext;
}UsartTxBuffer;

int32_t UsartInit();
int32_t UsartReceiveData(uint8_t* addr_dst, int32_t size);
int32_t UsartSendData(uint8_t* addr_src, int32_t size);
void USART1_IRQHandler();
#endif