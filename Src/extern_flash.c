#include "extern_flash.h" 

#define EXTERN_FLASH_PAGE_SIZE 256

static uint8_t externFlashBufferPing[EXTERN_FLASH_PAGE_SIZE];
static uint8_t externFlashBufferPong[EXTERN_FLASH_PAGE_SIZE];


int32_t ExternFlashInit(){
    //check if SPI2 is enable
    //if not, init spi2
    if(!LL_SPI_IsEnabled(SPI2))SpiInit();

}

static int32_t FlashConfig(){
    //Write Enable
    LL_SPI_TransmitData8(SPI2, 0x06);
    while(!LL_SPI_IsActiveFlag_TXE(SPI2));
    //Clear TX empty flag
    SPI2->SR &= (~LL_SPI_SR_TXE);
    //Write 0x00 to Status Reg 1 and Status Reg 2
    //Thus all page all writable
    LL_SPI_TransmitData8(SPI2, 0x01);
    while(!LL_SPI_IsActiveFlag_TXE(SPI2));
    SPI2->SR &= (~LL_SPI_SR_TXE);
    LL_SPI_TransmitData8(SPI2, 0x00);
    while(!LL_SPI_IsActiveFlag_TXE(SPI2));
    SPI2->SR &= (~LL_SPI_SR_TXE);
    LL_SPI_TransmitData8(SPI2, 0x00);
    while(!LL_SPI_IsActiveFlag_TXE(SPI2));
    SPI2->SR &= (~LL_SPI_SR_TXE);




}