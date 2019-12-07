#include "i2c.h"

struct
{
    uint32_t direction;
    uint8_t deviceAddr;
    uint8_t* addrSrc;
    uint8_t* addrDst;
    uint32_t cntSrc;
    uint32_t cntDst;
} I2cTransmitRequst;

int32_t I2cInit()
{
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)) {
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    //GPIO AF configuration
    //I2C2 SCL is PB10, I2C2 SDA is PB11, configure into af output
    LL_GPIO_InitTypeDef i2c2GpioInit;
    i2c2GpioInit.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
    i2c2GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    i2c2GpioInit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    i2c2GpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    i2c2GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &i2c2GpioInit);

    //enable i2c2 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

    LL_I2C_InitTypeDef i2c2Init;
    LL_I2C_StructInit(&i2c2Init);
    i2c2Init.ClockSpeed = 200000u;
    i2c2Init.PeripheralMode = LL_I2C_MODE_I2C;
    i2c2Init.OwnAddress1 = 0x52;
    i2c2Init.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    i2c2Init.TypeAcknowledge = LL_I2C_ACK;
    LL_I2C_Init(I2C2, &i2c2Init);

    LL_I2C_EnableIT_EVT(I2C2);
    LL_I2C_EnableIT_BUF(I2C2);

    LL_I2C_Enable(I2C2);

    SetMystdioTarget(USART2);
    MyPrintf("I2c init success!\n");
    return 0;
}

uint8_t I2cWriteByte(uint8_t deviceAddr, uint8_t regAddr, uint8_t data)
{
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 0;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    while(!LL_I2C_IsActiveFlag_TXE(I2C2))continue;
    LL_I2C_TransmitData8(I2C2, regAddr);

    while(!LL_I2C_IsActiveFlag_TXE(I2C2))continue;
    LL_I2C_TransmitData8(I2C2, data);

    //wait until the last transfer complete
    while(!LL_I2C_IsActiveFlag_TXE(I2C2) || !LL_I2C_IsActiveFlag_BTF(I2C2))continue;

    LL_I2C_GenerateStopCondition(I2C2);
    //Wait until bus release
    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;

    return data;
}

uint8_t I2cReadByte(uint8_t deviceAddr, uint8_t regAddr)
{
    if(LL_I2C_IsActiveFlag_BUSY(I2C2))return 0;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    //Transfer device Addr, and wait for it complete
    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    //Transfer device reg addr, and stop this frame
    LL_I2C_TransmitData8(I2C2, regAddr);
    while(!LL_I2C_IsActiveFlag_TXE(I2C2) || !LL_I2C_IsActiveFlag_BTF(I2C2))continue;
    LL_I2C_GenerateStopCondition(I2C2);

    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;
    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))continue;

    //Transfer device Addr for read, and wait for it complete
    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) | I2C_REQUEST_READ);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))continue;
    LL_I2C_AcknowledgeNextData(I2C2, LL_I2C_NACK);
    LL_I2C_ClearFlag_ADDR(I2C2);

    LL_I2C_GenerateStopCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_RXNE(I2C2))continue;
    uint8_t data = LL_I2C_ReceiveData8(I2C2);

    //Wait until bus release
    while(LL_I2C_IsActiveFlag_BUSY(I2C2))continue;

    return data;
}
