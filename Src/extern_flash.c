#include "extern_flash.h"

#define EXTERN_FLASH_PAGE_SIZE 128

static uint8_t externFlashBufferPing[EXTERN_FLASH_PAGE_SIZE];
static uint8_t externFlashBufferPong[EXTERN_FLASH_PAGE_SIZE];

static int32_t FlashUnlock()
{
    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    //Write 0x00 to Status Reg 1 and Status Reg 2
    //Thus all page all writable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG);
    SpiWriteReadByte(0x00);
    SpiWriteReadByte(0x00);
    SpiSSDisable();

    return 0;
}

static int32_t FlashLock()
{
    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    //protect all pages
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG);
    SpiWriteReadByte(0x00);
    SpiWriteReadByte(0x40);
    SpiSSDisable();

    return 0;
}

static int32_t FlashIsBusy()
{
    uint8_t flashStatus;
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_READREG1);
    flashStatus = SpiWriteReadByte(0xff);
    SpiSSDisable();

    return (flashStatus & 0x1);
}

static int32_t FlashSectorErase(uint32_t addr)
{
    //check if addr is legal
    if(addr & 0xfff) return 1;

    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    //erase
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_SECTORERASE);
    SpiWriteReadByte((uint8_t)(addr >> 16));
    SpiWriteReadByte((uint8_t)(addr >> 8));
    SpiWriteReadByte((uint8_t)addr);
    SpiSSDisable();

    //wait util erase complete
    while(FlashIsBusy())
        ;
    return 0;
}

static int32_t FlashReadByte(uint32_t addr, uint8_t* addr_dst)
{
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_READBYTE);
    SpiWriteReadByte((uint8_t)(addr >> 16));
    SpiWriteReadByte((uint8_t)(addr >> 8));
    SpiWriteReadByte((uint8_t)addr);
    *addr_dst = SpiWriteReadByte(0xff);
    SpiSSDisable();

    return 0;
}

static int32_t FlashWrite(uint32_t addr, uint8_t* addr_src, uint32_t size)
{
    uint32_t i;
    if(size > 256) return 1;

    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiWriteReadByte((uint8_t)(addr >> 16));
    SpiWriteReadByte((uint8_t)(addr >> 8));
    SpiWriteReadByte((uint8_t)addr);

    for(i = 0; i < size; i++) {
        SpiWriteReadByte(addr_src[i]);
    }
    SpiSSDisable();

    return 0;
}

static int32_t FlashSelfCheck()
{
    uint8_t flashSelfCheck[2];
    //Read the first two byte
    FlashReadByte(0x0, (uint8_t*)flashSelfCheck);
    FlashReadByte(0x1, (uint8_t*)(flashSelfCheck + 1));

    if(flashSelfCheck[0] == 0x10 && flashSelfCheck[1] == 0x52)
        return 0;
    else
        return 1;
}

static int32_t FlashConfig()
{

    FlashUnlock();
    //Erase the first sector 0x000000
    FlashSectorErase(0x000000);

    //write data to page 0
    uint8_t defaultData[2] = {0x10, 0x52};
    FlashWrite(0x0, (uint8_t*)defaultData, 2);

    //complete config
    FlashLock();

    return 0;
}

int32_t ExternFlashInit()
{
    //check if SPI2 is enable
    //if not, init spi2
    if(!LL_SPI_IsEnabled(SPI2)) SpiInit();
    if(FlashSelfCheck()) {
        FlashConfig();
        if(FlashSelfCheck())
            return 1;
        else
            return 0;
    }
    else
        return 0;
}
