#include <stdio.h>
#include "mystdio.h"

USART_TypeDef* usartTarget = USART2;

int32_t SetMystdioTarget(USART_TypeDef* mystdioUsartTarget)
{
    usartTarget = mystdioUsartTarget;
    return 0;
}

int32_t _write(int32_t ch, uint8_t* pBuffer, int32_t size)
{
    int32_t sizeSent = 0;
    sizeSent += UsartSendData(pBuffer, size, usartTarget);
    while(sizeSent < size) {
        sizeSent += UsartSendData(pBuffer + sizeSent, size - sizeSent, usartTarget);
    };
    return size;
}

int32_t _read(int32_t ch, uint8_t* pBuffer, int32_t size)
{
    int32_t sizeReceived = 0;
    sizeReceived += UsartReceiveData(pBuffer, size, usartTarget);
    while(sizeReceived < size) {
        sizeReceived += UsartReceiveData(pBuffer + sizeReceived, size - sizeReceived, usartTarget);
    };
    return size;
}

int32_t MyPrintf(const char *__format, ...)
{
  int32_t __retval;
  __builtin_va_list __local_argv; 
  __builtin_va_start(__local_argv, __format);
  __retval = (int32_t)__builtin_vfprintf(stdout, __format, __local_argv);
  __builtin_va_end(__local_argv);
  return __retval;
}

int32_t MyScanf(const char *__format, ...)
{
  int32_t __retval;
  __builtin_va_list __local_argv; 
  __builtin_va_start(__local_argv, __format);
  __retval = (int32_t)__builtin_vfscanf(stdin, __format, __local_argv);
  __builtin_va_end(__local_argv);
  return __retval;
}

uint8_t MyGetchar()
{
    while(MystdinBufferIsEmpty()) continue;
    uint8_t ch;
    UsartReceiveData(&ch, 1, usartTarget);
    return ch;
}

uint8_t MyPutchar(uint8_t ch)
{
    while(MystdoutBufferIsFull()) continue;
    UsartSendData(&ch, 1, usartTarget);
    return ch;
}

int32_t MystdinBufferIsEmpty()
{
    return UsartRxBufferIsEmpty(usartTarget);
}

int32_t MystdoutBufferIsFull()
{
    return UsartTxBufferIsFull(usartTarget);
}
