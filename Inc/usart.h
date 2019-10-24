#ifndef USART_H
#define USART_H

#include "stm32f103xb.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

#define RX_BUFFER_SIZE 16

typedef struct{
    uint8_t *addr;
    uint32_t cnt;
    uint32_t busy;
}UsartSendRequest;

typedef struct{
    uint8_t addr[RX_BUFFER_SIZE];
    uint32_t head;
    uint32_t tail;
}RxBuffer;

int UsartInit();
int UsartSendData(uint8_t *addr_src, uint32_t cnt);
int UsartReadBuffer(uint8_t *addr_dst);
void USART1_IRQHandler();
#endif