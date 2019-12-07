#ifndef MPU6050_H
#define MPU6050_H

#include "mystdio.h"
#include "i2c.h"

#define MPU_ADDRESS 0x69

#define MPU_SMPRTDIV 0x19
#define MPU_CONFIG 0x1A
#define MPU_GYROCONFIG 0x1B
#define MPU_ACCELCONFIG 0x1C
#define MPU_PWRREG1 0x6B

#define MPU_SLEF_TEST_X 0x0D
#define MPU_SLEF_TEST_Y 0x0E
#define MPU_SLEF_TEST_Z 0x0F
#define MPU_SLEF_TEST_A 0x10

#define MPU_ACCEL_XOUT_H 0x3B
#define MPU_ACCEL_XOUT_L 0x3C
#define MPU_ACCEL_YOUT_H 0x3D
#define MPU_ACCEL_YOUT_L 0x3E
#define MPU_ACCEL_ZOUT_H 0x3F
#define MPU_ACCEL_ZOUT_L 0x40

#define MPU_TEMP_OUT_H 0x41
#define MPU_TEMP_OUT_L 0x42

#define MPU_GYRO_XOUT_H 0x43 
#define MPU_GYRO_XOUT_L 0x44
#define MPU_GYRO_YOUT_H 0x45
#define MPU_GYRO_YOUT_L 0x46
#define MPU_GYRO_ZOUT_H 0x47
#define MPU_GYRO_ZOUT_L 0x48

#define MPU_WHO_AM_I 0x75
#define MPU_WHO_AM_I_BULIDIN_DATA 0x68

int32_t MpuInit();
int16_t MpuGetData(uint8_t regAddr);

#endif
