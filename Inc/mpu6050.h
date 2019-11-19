#ifndef MPU6050_H
#define MPU6050_H

#include "i2c.h"

#define MPU_ADDRESS 0x68

#define MPU_SMPRTDIV 0x19
#define MPU_CONFIG 0x1A
#define MPU_GYROCONFIG 0x1B
#define MPU_ACCELCONFIG 0x1C
#define MPU_PWRREG1 0x6B

int32_t MpuInit();

#endif
