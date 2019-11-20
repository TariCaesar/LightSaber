#ifndef MPU6050_H
#define MPU6050_H

#include "i2c.h"

#define MPU_ADDRESS 0x68

#define MPU_SMPRTDIV 0x19
#define MPU_CONFIG 0x1A
#define MPU_GYROCONFIG 0x1B
#define MPU_ACCELCONFIG 0x1C
#define MPU_PWRREG1 0x6B

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42

#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

void MpuInit();
int16_t MpuGetData(uint8_t regAddr);

#endif
