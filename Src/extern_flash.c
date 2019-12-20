#include "extern_flash.h"

static struct{
    uint32_t addr;
    uint8_t* addrDst;
    uint32_t size;
    uint32_t cnt;
}flashFastReadTask;

static int32_t FlashIsBusy()
{
    uint8_t flashStatus;
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_READREG1);
    flashStatus = Spi1WriteReadByte(0xff);
    Spi1SSDisable();

    return (flashStatus & 0x1);
}

static int32_t FlashUnlock()
{
    //Write Enable
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEENABLE);
    Spi1SSDisable();

    //Write 0x00 to Status Reg 1 and Status Reg 2
    //Thus all page all writable
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEREG1);
    Spi1WriteReadByte(0x60);
    Spi1WriteReadByte(0x00);
    Spi1SSDisable();
    while(FlashIsBusy())continue;

    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEREG3);
    Spi1WriteReadByte(0x60);
    Spi1SSDisable();
    while(FlashIsBusy())continue;

    return 0;
}

static int32_t FlashLock()
{
    //Write Enable
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEENABLE);
    Spi1SSDisable();

    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEREG1);
    Spi1WriteReadByte(0x60);
    Spi1WriteReadByte(0x40);
    Spi1SSDisable();
    while(FlashIsBusy())continue;

    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEREG3);
    Spi1WriteReadByte(0x60);
    Spi1SSDisable();
    while(FlashIsBusy())continue;

    return 0;
}

static int32_t FlashSectorErase(uint32_t addr)
{
    //check if addr is legal
    if(addr & 0xfff) return 1;

    //Write Enable
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEENABLE);
    Spi1SSDisable();

    //erase
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_SECTORERASE);
    Spi1WriteReadByte((uint8_t)(addr >> 16));
    Spi1WriteReadByte((uint8_t)(addr >> 8));
    Spi1WriteReadByte((uint8_t)addr);
    Spi1SSDisable();

    //wait util erase complete
    while(FlashIsBusy())continue;
    return 0;
}

static int32_t FlashWritePage(uint32_t addr, uint8_t* addrSrc, uint32_t size)
{
    uint32_t i;
    if(size > 256) return 1;

    //Write Enable
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEENABLE);
    Spi1SSDisable();

    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_WRITEPAGE);
    Spi1WriteReadByte((uint8_t)(addr >> 16));
    Spi1WriteReadByte((uint8_t)(addr >> 8));
    Spi1WriteReadByte((uint8_t)addr);

    for(i = 0; i < size; i++) {
        Spi1WriteReadByte(addrSrc[i]);
    }
    Spi1SSDisable();

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
    UsartSetMystdioHandler(USART2);
    MyPrintf("Start writing data to flash.\n");
    FlashUnlock();
    int32_t sectorNum = size / EXTERN_FLASH_SECTOR_SIZE;
    for(int32_t i = 0; i < sectorNum; ++i){
        uint32_t sectorStartAddr = (sectorOffset + i) * EXTERN_FLASH_SECTOR_SIZE;
        FlashSectorErase(sectorStartAddr);
        for(int32_t j = 0; j < EXTERN_FLASH_SECTOR_SIZE / EXTERN_FLASH_PAGE_SIZE; ++j){
            FlashWritePage(
                sectorStartAddr + j * EXTERN_FLASH_PAGE_SIZE, 
                data + i * EXTERN_FLASH_SECTOR_SIZE + j * EXTERN_FLASH_PAGE_SIZE, 
                EXTERN_FLASH_PAGE_SIZE
            );
        }
        UsartSetMystdioHandler(USART2);
        MyPrintf("Sector %d, starts from addr %05X, has been written.\n", sectorOffset + i, sectorStartAddr);
    }
    FlashLock();

    //Verify
    uint8_t dataBuffer[EXTERN_FLASH_SECTOR_SIZE];
    for(int32_t i = 0; i < sectorNum; ++i){
        uint32_t sectorStartAddr = (sectorOffset + i) * EXTERN_FLASH_SECTOR_SIZE;
        FlashRead(sectorStartAddr, dataBuffer, EXTERN_FLASH_SECTOR_SIZE);
        for(int32_t j = 0; j < EXTERN_FLASH_SECTOR_SIZE; ++j){
            if(dataBuffer[j] != data[j + i * EXTERN_FLASH_SECTOR_SIZE]){
                UsartSetMystdioHandler(USART2);
                MyPrintf("Sector %d, starts from addr %05X, verification fail.\n", sectorOffset + i, sectorStartAddr);
                return 1;
            }
        }
        MyPrintf("Sector %d, starts from addr %05X, verification success.\n", sectorOffset + i, sectorStartAddr);
    }
    UsartSetMystdioHandler(USART2);
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

uint32_t FlashRead(uint32_t addr, uint8_t* addrDst, uint32_t size)
{
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_FASTREAD);
    Spi1WriteReadByte((uint8_t)(addr >> 16));
    Spi1WriteReadByte((uint8_t)(addr >> 8));
    Spi1WriteReadByte((uint8_t)addr);
    //dummy frame
    Spi1WriteReadByte(0xff);
    for(int32_t i = 0; i < size; ++i){
        addrDst[i] = Spi1WriteReadByte(0xff);
    }
    Spi1SSDisable();

    return size;
}

int32_t FlashFastRead(uint32_t addr, uint8_t* addrDst, uint32_t size){
    Spi1SSEnable();
    Spi1WriteReadByte(FLASH_CMD_FASTREAD);
    Spi1WriteReadByte((uint8_t)(addr >> 16));
    Spi1WriteReadByte((uint8_t)(addr >> 8));
    Spi1WriteReadByte((uint8_t)addr);
    //dummy frame
    Spi1WriteReadByte(0xff);
    Spi1WriteReadDMA(0, addrDst, size, 0);
    return 0;
}

int32_t ExternFlashInit()
{
    Spi1Init();

    if(FlashSelfCheck()) {
        UsartSetMystdioHandler(USART2);
        MyPrintf("External flash self check fail!\n");
        return 1;
    }
    else {
        UsartSetMystdioHandler(USART2);
        MyPrintf("External flash self check pass!\n");
        return 0;
    }
}

