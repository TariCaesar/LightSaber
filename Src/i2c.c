#include "i2c.h"

int32_t I2cInit(){
    //check the GPIOB clock status
    //if not open, open it
    if(!LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_GPIOB)){
        LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    }

    //GPIO AF configuration
    //I2C1 SCL is PB6, I2C2 SDA is PB7, configure into af output
    LL_GPIO_InitTypeDef i2c1GpioInit;
    i2c1GpioInit.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    i2c1GpioInit.Mode = LL_GPIO_MODE_ALTERNATE;
    i2c1GpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    i2c1GpioInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
    i2c1GpioInit.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOB, &i2c1GpioInit); 

    //enable i2c1 clock
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_I2C_InitTypeDef i2c1Init;
    LL_I2C_StructInit(&i2c1Init);
    i2c1Init.ClockSpeed = 100000u;
    i2c1Init.PeripheralMode = LL_I2C_MODE_I2C;
    i2c1Init.OwnAddress1 = 0x52;
    i2c1Init.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
    i2c1Init.TypeAcknowledge = LL_I2C_ACK;
    LL_I2C_Init(I2C1, &i2c1Init);

    LL_I2C_EnableIT_TX(I2C1);

    LL_I2C_Enable(I2C1);

    return 0;
}

uint32_t I2cWrite(uint8_t deviceAddr, uint8_t* addrSrc, uint32_t dataSize){
    uint32_t i;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1))continue;

    //Transfer device Addr, and wait for it complete
    LL_I2C_TransmitData8(I2C1, (deviceAddr << 1) & I2C_REQUEST_WRITE);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C1))continue;
    LL_I2C_ClearFlag_ADDR(I2C1);

    //Transfer data
    for(i = 0; i < dataSize; ++i){
        LL_I2C_TransmitData8(I2C1, addrSrc[i]);
        while(!LL_I2C_IsActiveFlag_TXE(I2C1))continue;
    }

    //Generate Stop Signal, and wait for it complete
    LL_I2C_GenerateStopCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1))continue;
    LL_I2C_ClearFlag_STOP(I2C1);

    return dataSize;
}

uint32_t I2cRead(uint8_t deviceAddr, uint8_t* addrDst, uint32_t dataSize){
    uint32_t i;

    //Generate Start Signal, and wait for it complete
    LL_I2C_GenerateStartCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_SB(I2C1))continue;

    //Transfer device Addr, and wait for it complete
    LL_I2C_TransmitData8(I2C1, (deviceAddr << 1) | I2C_REQUEST_READ);
    while(!LL_I2C_IsActiveFlag_ADDR(I2C1))continue;
    LL_I2C_ClearFlag_ADDR(I2C1);

    //Transfer data
    for(i = 0; i < dataSize; ++i){
        while(!LL_I2C_IsActiveFlag_RXNE(I2C1));
        addrDst[i] = LL_I2C_ReceiveData8(I2C1);
    }

    //Generate Stop Signal, and wait for it complete
    LL_I2C_GenerateStopCondition(I2C1);
    while(!LL_I2C_IsActiveFlag_STOP(I2C1))continue;
    LL_I2C_ClearFlag_STOP(I2C1);

    return dataSize;
}
