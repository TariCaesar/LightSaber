#include "extern_flash.h" 

#define EXTERN_FLASH_PAGE_SIZE 4096

static uint8_t externFlashBufferPing[EXTERN_FLASH_PAGE_SIZE];
static uint8_t externFlashBufferPong[EXTERN_FLASH_PAGE_SIZE];


int ExternFlashInit(){
    //check if SPI2 is enable
    //if not, init spi2
    if(!LL_SPI_IsEnabled(SPI2))SpiInit();
}