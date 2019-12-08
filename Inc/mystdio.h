#ifndef MYSTDIO_H
#define MYSTDIO_H

int32_t SetMystdioTransimitHandler(uint32_t (*handler)(uint8_t*, uint32_t));
int32_t SetMystdioReceiveHandler(uint32_t (*handler)(uint8_t*, uint32_t));

int32_t MyPrintf(const char*, ...);
int32_t MyScanf(const char*, ...);

uint8_t MyGetchar();
uint8_t MyPutchar(uint8_t ch);

int32_t MystdoutBufferIsFull();
int32_t MystdinBufferIsEmpty();

#endif
