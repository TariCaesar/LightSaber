#include "mpu6050.h"

static int32_t MpuWriteReg(uint8_t regAddr, uint8_t data)
{
    uint8_t dataSequence[2];
    dataSequence[0] = regAddr;
    dataSequence[1] = data;
    I2cWrite(MPU_ADDRESS, dataSequence, 2);
    return 0;
}

static uint8_t MpuReadReg(uint8_t regAddr)
{
    return I2cRead(MPU_ADDRESS, regAddr);
}

int16_t MpuGetData(uint8_t regAddr)
{
    uint8_t Data_H, Data_L;
    uint16_t data;

    Data_H = MpuReadReg(regAddr);
    Data_L = MpuReadReg(regAddr + 1);
    data = (Data_H << 8) | Data_L;

    return data;
}

int32_t MpuInit()
{
    //check if I2C2 is enable
    //if not, init I2C2
    if(!LL_I2C_IsEnabled(I2C2))
        I2cInit();

    //init mpu6050
    //reset mpu6050
    MpuWriteReg(MPU_PWRREG1, 0x80);
    while(MpuReadReg(MPU_PWRREG1) & 0x80)
        continue;

    //set mpu6050 into nonsleepmode, choose X gyroscope as source
    MpuWriteReg(MPU_PWRREG1, 0x05);
    MpuWriteReg(MPU_SMPRTDIV, 0x04);
    MpuWriteReg(MPU_CONFIG, 0x01);
    //set full range to 250degree and 2g
    MpuWriteReg(MPU_GYROCONFIG, 0x00);
    MpuWriteReg(MPU_ACCELCONFIG, 0x00);
    return 0;
}
