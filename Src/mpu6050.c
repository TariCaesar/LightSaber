#include "mpu6050.h"

static inline uint8_t MpuWriteReg(uint8_t regAddr, uint8_t data)
{
    return I2cWriteByte(MPU_ADDRESS, regAddr, data);
}

static inline uint8_t MpuReadReg(uint8_t regAddr)
{
    return I2cReadByte(MPU_ADDRESS, regAddr);
}

int32_t MpuInit()
{
    //check if I2C2 is enable
    //if not, init I2C2
    if(!LL_I2C_IsEnabled(I2C2))
        I2cInit();

    //init mpu6050
    //wake and reset mpu6050
    MpuWriteReg(MPU_PWRREG1, 0x80);
    while(MpuReadReg(MPU_PWRREG1) & 0x80)continue;

    //set mpu6050 into nonsleepmode, choose X gyroscope as source
    MpuWriteReg(MPU_SMPRTDIV, 0x04);
    MpuWriteReg(MPU_CONFIG, 0x01);

    //set full range to +-500degree and +-4g
    MpuWriteReg(MPU_GYROCONFIG, 0x04);
    MpuWriteReg(MPU_ACCELCONFIG, 0x04);

    if(MpuReadReg(MPU_WHO_AM_I) == MPU_WHO_AM_I_BULIDIN_DATA){
        SetMystdioTarget(USART2);
        MyPrintf("Mpu6050 init success!\n");
        return 0;
    }
    else{
        SetMystdioTarget(USART2);
        MyPrintf("Mpu6050 init failed!\n");
        return 1;
    }
}
