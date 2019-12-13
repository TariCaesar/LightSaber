#ifndef EXTERN_FLASH_H
#define EXTERN_FLASH_H

#include "system.h"
#include "mystdio.h"
#include "usart.h"
#include "spi.h"

#define EXTERN_FLASH_PAGE_SIZE 256u
#define EXTERN_FLASH_SECTOR_SIZE 4096u

#define FLASH_CMD_WRITEENABLE 0x06
#define FLASH_CMD_WRITEDISABLE 0x04
#define FLASH_CMD_WRITEREG1 0x01
#define FLASH_CMD_WRITEREG2 0x31
#define FLASH_CMD_WRITEREG3 0x11
#define FLASH_CMD_READREG1 0x05
#define FLASH_CMD_READREG2 0x35
#define FLASH_CMD_READREG3 0x15
#define FLASH_CMD_SECTORERASE 0x20
#define FLASH_CMD_READ 0x03
#define FLASH_CMD_FASTREAD 0x0B
#define FLASH_CMD_WRITEPAGE 0x02

int32_t ExternFlashInit();
uint32_t FlashRead(uint32_t addr, uint8_t* addrDst, uint32_t size);
int32_t FlashFastRead(uint32_t addr, uint8_t* addrDst, uint32_t size);

#endif
