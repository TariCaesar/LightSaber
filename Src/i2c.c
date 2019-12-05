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
    i2c2Init.ClockSpeed = 100000u;
    i2c2Init.PeripheralMode = LL_I2C_MODE_I2C;
    i2c2Init.OwnAddress1 = 0x52;
    i2c2Init.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    i2c2Init.TypeAcknowledge = LL_I2C_ACK;
    LL_I2C_Init(I2C2, &i2c2Init);

    LL_I2C_EnableIT_EVT(I2C2);
    LL_I2C_EnableIT_RX(I2C2);
    LL_I2C_EnableIT_TX(I2C2);

    //enabel irq for i2c2
    NVIC_SetPriority(I2C2_EV_IRQn, NVIC_EncodePriority(2, 3, 0));
    NVIC_EnableIRQ(I2C2_EV_IRQn);

    LL_I2C_Enable(I2C2);

    return 0;
}

uint32_t I2cWrite(uint8_t deviceAddr, uint8_t* addrSrc, uint32_t dataSize)
{
    uint32_t i;
    if(LL_I2C_IsActiveFlag_BUSY(I2C2)) return 0;

    I2cTransmitRequst.direction = I2C_REQUEST_WRITE;
    I2cTransmitRequst.deviceAddr = deviceAddr;
    I2cTransmitRequst.addrSrc = addrSrc;
    I2cTransmitRequst.cntSrc = dataSize;
    LL_I2C_GenerateStartCondition(I2C2);
}

uint8_t I2cRead(uint8_t deviceAddr, uint8_t regAddr)
{
    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))
        continue;

    //Transfer device Addr, and wait for it complete
    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))
        continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    LL_I2C_TransmitData8(I2C2, regAddr);
    while(!LL_I2C_IsActiveFlag_TXE(I2C2))
        continue;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_SB(I2C2))
        continue;

    //Transfer device Addr for read, and wait for it complete
    LL_I2C_TransmitData8(I2C2, (deviceAddr << 1) | I2C_REQUEST_READ);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C2))
        continue;
    LL_I2C_ClearFlag_ADDR(I2C2);

    while(!LL_I2C_IsActiveFlag_RXNE(I2C2))
        continue;
    uint8_t data = LL_I2C_ReceiveData8(I2C2);

    //Generate Stop Signal, and wait for it complete
    LL_I2C_GenerateStopCondition(I2C2);
    while(!LL_I2C_IsActiveFlag_STOP(I2C2))
        continue;
    LL_I2C_ClearFlag_STOP(I2C2);

    return data;
}

void I2C2_EV_IRQHandler()
{
    if(LL_I2C_IsActiveFlag_SB(I2C2)) {
        LL_I2C_TransmitData8(I2C2, (I2cTransmitRequst.deviceAddr << 1) & I2C_REQUEST_WRITE);
    }
    else if(LL_I2C_IsActiveFlag_ADDR(I2C2)) {
        LL_I2C_TransmitData8(I2C2, (I2cTransmitRequst.addrSrc++));
        --I2cTransmitRequst.cntSrc;
    }
    else if(LL_I2C_IsActiveFlag_TXE(I2C2)) {
        if(I2cTransmitRequst.cntSrc == 0) {
            LL_I2C_GenerateStopCondition(I2C2);
        }
        else {
            LL_I2C_TransmitData8(I2C2, (I2cTransmitRequst.addrSrc++));
            --I2cTransmitRequst.cntSrc;
        }
    }
}
