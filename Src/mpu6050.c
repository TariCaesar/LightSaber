#include "mpu6050.h"

int16_t gyroData[3] = {0};
static uint32_t mpuDataUpdateCnt = 0;

static inline uint16_t MpuWriteReg(uint8_t regAddr, uint8_t data)
{
    return I2cWriteHalfWord(MPU_ADDRESS, ((((uint16_t)regAddr) << 8) | (uint16_t)data));
}

static inline uint8_t MpuReadReg(uint8_t regAddr)
{
    //first write the regAddr want to read
    I2cWriteByte(MPU_ADDRESS, regAddr);
    return I2cReadByte(MPU_ADDRESS);
}

static uint8_t mpuGyroXHAddr = MPU_GYRO_XOUT_H;
static uint8_t mpuGyroXLAddr = MPU_GYRO_XOUT_L;
static uint8_t mpuGyroYHAddr = MPU_GYRO_YOUT_H;
static uint8_t mpuGyroYLAddr = MPU_GYRO_YOUT_L;
static uint8_t mpuGyroZHAddr = MPU_GYRO_ZOUT_H;
static uint8_t mpuGyroZLAddr = MPU_GYRO_ZOUT_L;

static I2C_TASK mpuDataUpdateTaskWrap[] = {
    {MPU_ADDRESS, &mpuGyroXHAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData + 1, I2C_REQUEST_READ},
    {MPU_ADDRESS, &mpuGyroXLAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData, I2C_REQUEST_READ},
    {MPU_ADDRESS, &mpuGyroYHAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData + 3, I2C_REQUEST_READ},
    {MPU_ADDRESS, &mpuGyroYLAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData + 2, I2C_REQUEST_READ},
    {MPU_ADDRESS, &mpuGyroZHAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData + 5, I2C_REQUEST_READ},
    {MPU_ADDRESS, &mpuGyroZLAddr, I2C_REQUEST_WRITE},
    {MPU_ADDRESS, (uint8_t*)gyroData + 4, I2C_REQUEST_READ}
};

void MpuDataUpdate(){
    I2cTransferWrap(mpuDataUpdateTaskWrap, 12);
    mpuDataUpdateCnt += 1;
    if(mpuDataUpdateCnt == 100){
        mpuDataUpdateCnt = 0;
        UsartSetMystdioHandler(USART2);
        MyPrintf(
            "Mpu gyro data: x:%d y:%d z:%d\n", 
            gyroData[0], 
            gyroData[1], 
            gyroData[2]
        );
    }
    return;
}

int32_t MpuInit()
{
    I2cInit();

    //init mpu6050
    //wake and reset mpu6050
    MpuWriteReg(MPU_PWRREG1, 0x80);
    
    //wait for mpu6050 reset complete
    do{
        SysDelayMs(10);       
    }while(MpuReadReg(MPU_PWRREG1) & 0x80);

    //Wake up mpu, set clock to internal clock and disable temperature sensor
    MpuWriteReg(MPU_PWRREG1, 0x08);

    //set mpu6050 into nonsleepmode, choose X gyroscope as source
    MpuWriteReg(MPU_SMPRTDIV, 0x04);
    MpuWriteReg(MPU_CONFIG, 0x01);

    //set full range to +-500degree and +-4g
    MpuWriteReg(MPU_GYROCONFIG, 0x04);
    MpuWriteReg(MPU_ACCELCONFIG, 0x04);

    if(MpuReadReg(MPU_WHO_AM_I) == MPU_WHO_AM_I_BULIDIN_DATA){
        UsartSetMystdioHandler(USART2);
        MyPrintf("Mpu6050 initialization succeed!\n");
        EnableMpuDataUpdate();
        return 0;
    }
    else{
        UsartSetMystdioHandler(USART2);
        MyPrintf("Mpu6050 initialization fail!\n");
        return 1;
    }
}

int32_t EnableMpuDataUpdate(){
    //Init tim2 for update
    Timer2Init(MpuDataUpdate, 100);
    Timer2Enable();
    MyPrintf("Start mpu data update.\n");
    return 0;
}
