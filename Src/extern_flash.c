#include "extern_flash.h"

#include "sound_open.h"

static int32_t FlashIsBusy()
{
    uint8_t flashStatus;
    for(int32_t i = 0; i < 3600000; ++i)continue;
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_READREG1);
    flashStatus = SpiWriteReadByte(0xff);
    SpiSSDisable();

    return (flashStatus & 0x1);
}

static int32_t FlashUnlock()
{
    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    //Write 0x00 to Status Reg 1 and Status Reg 2
    //Thus all page all writable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG1);
    SpiWriteReadByte(0x60);
    SpiWriteReadByte(0x00);
    SpiSSDisable();
    while(FlashIsBusy())continue;

    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG3);
    SpiWriteReadByte(0x60);
    SpiSSDisable();
    while(FlashIsBusy())continue;

    return 0;
}

static int32_t FlashLock()
{
    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG1);
    SpiWriteReadByte(0x60);
    SpiWriteReadByte(0x40);
    SpiSSDisable();
    while(FlashIsBusy())continue;

    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEREG3);
    SpiWriteReadByte(0x60);
    SpiSSDisable();
    while(FlashIsBusy())continue;

    return 0;
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
    while(FlashIsBusy())continue;
    return 0;
}

static int32_t FlashRead(uint32_t addr, uint8_t* addrDst, int32_t size)
{
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_FASTREAD);
    SpiWriteReadByte((uint8_t)(addr >> 16));
    SpiWriteReadByte((uint8_t)(addr >> 8));
    SpiWriteReadByte((uint8_t)addr);
    //dummy frame
    SpiWriteReadByte(0xff);
    for(int32_t i = 0; i < size; ++i){
        addrDst[i] = SpiWriteReadByte(0xff);
    }
    SpiSSDisable();

    return 0;
}

static int32_t FlashWritePage(uint32_t addr, uint8_t* addrSrc, uint32_t size)
{
    uint32_t i;
    if(size > 256) return 1;

    //Write Enable
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEENABLE);
    SpiSSDisable();

    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_WRITEPAGE);
    SpiWriteReadByte((uint8_t)(addr >> 16));
    SpiWriteReadByte((uint8_t)(addr >> 8));
    SpiWriteReadByte((uint8_t)addr);

    for(i = 0; i < size; i++) {
        SpiWriteReadByte(addrSrc[i]);
    }
    SpiSSDisable();

    //wait util write complete
    while(FlashIsBusy())continue;

    return 0;
}

static int32_t FlashSelfCheck()
{
    uint8_t flashSelfCheck[2];
    //Read the first two byte
    FlashRead(0x0, (uint8_t*)flashSelfCheck, 2);

    if(flashSelfCheck[0] == 0x10 && flashSelfCheck[1] == 0x52)
        return 0;
    else
        return 1;
}

static int32_t FlashWrite(uint8_t* data, uint32_t size, int32_t sectorOffset){
    MyPrintf("Start writing data to flash\n");
    FlashUnlock();
    int32_t sectorNum = size / EXTERN_FLASH_SECTOR_SIZE;
    for(int32_t i = 0; i < sectorNum; ++i){
        uint32_t sectorStartAddr = (sectorOffset + i) * EXTERN_FLASH_SECTOR_SIZE;
        FlashSectorErase(sectorStartAddr);
        for(int32_t j = 0; j < EXTERN_FLASH_SECTOR_SIZE / EXTERN_FLASH_PAGE_SIZE; ++j){
            FlashWritePage(sectorStartAddr + j * EXTERN_FLASH_PAGE_SIZE, data + j * EXTERN_FLASH_PAGE_SIZE, EXTERN_FLASH_PAGE_SIZE);
        }
        MyPrintf("Sector %d, starts from addr %05X, has been written\n", sectorOffset + i, sectorStartAddr);
    }

    FlashLock();

    uint8_t flashStatus;
    SpiSSEnable();
    SpiWriteReadByte(FLASH_CMD_READREG2);
    flashStatus = SpiWriteReadByte(0xff);
    SpiSSDisable();

    //Verify
    uint8_t dataBuffer[EXTERN_FLASH_SECTOR_SIZE];
    for(int32_t i = 0; i < sectorNum; ++i){
        uint32_t sectorStartAddr = (sectorOffset + i) * EXTERN_FLASH_SECTOR_SIZE;
        FlashRead(sectorStartAddr, dataBuffer, EXTERN_FLASH_SECTOR_SIZE);
        for(int32_t j = 0; j < EXTERN_FLASH_SECTOR_SIZE; ++j){
            if(dataBuffer[j] != data[j + i * EXTERN_FLASH_SECTOR_SIZE]){
                MyPrintf("Sector %d, starts from addr %05X, verification fail\n", sectorOffset + i, sectorStartAddr);
                return 1;
            }
        }
    }

    MyPrintf("All sectors verified!\n");
    return 0;
}

static int32_t FlashConfig()
{

    FlashUnlock();
    //Erase the first sector 0x000000
    FlashSectorErase(0x000000);

    //write data to page 0
    uint8_t defaultData[2] = {0x10, 0x52};
    FlashWritePage(0x0, (uint8_t*)defaultData, 2);

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
        SetMystdioTarget(USART2);
        MyPrintf("External Flash self check fail!\n");
        return 1;
    }
    else {
        SetMystdioTarget(USART2);
        MyPrintf("External Flash self check pass!\n");
        //FlashWrite((uint8_t*)data, sizeof(data), 1);
        return 0;
    }
}
