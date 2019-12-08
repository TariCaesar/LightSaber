#include <stdio.h>
#include "mystdio.h"

static uint32_t (*mystdioTransmitHandler)(uint8_t*, uint32_t);
static uint32_t (*mystdioReceiveHandler)(uint8_t*, uint32_t);

int32_t SetMystdioTransimitHandler(uint32_t (*handler)(uint8_t*, uint32_t)){
    mystdioTransmitHandler = handler;
    return 0;
}

int32_t SetMystdioReceiveHandler(uint32_t (*handler)(uint8_t*, uint32_t)){
    mystdioReceiveHandler = handler;
    return 0;
}

int32_t _write(int32_t ch, uint8_t* pBuffer, int32_t size)
{
    int32_t sizeSent = 0;
    sizeSent += mystdioTransmitHandler(pBuffer, size);
    while(sizeSent < size) {
        sizeSent += mystdioTransmitHandler(pBuffer + sizeSent, size - sizeSent);
    };
    return size;
}

int32_t _read(int32_t ch, uint8_t* pBuffer, int32_t size)
{
    int32_t sizeReceived = 0;
    sizeReceived += mystdioReceiveHandler(pBuffer, size);
    while(sizeReceived < size) {
        sizeReceived += mystdioReceiveHandler(pBuffer + sizeReceived, size - sizeReceived);
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
    uint8_t ch;
    while(mystdioReceiveHandler(&ch, 1) == 0) continue;
    return ch;
}

uint8_t MyPutchar(uint8_t ch)
{
    while(mystdioTransmitHandler(&ch, 1) == 0) continue;
    return ch;
}
