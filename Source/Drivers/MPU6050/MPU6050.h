#ifndef _MPU6050_H_
#define _MPU6050_H_
#include "Source/Drivers/i2c_rw.h"
// #include "Source/Drivers/MPU6050/MPU6050_reg.h"

typedef enum {
    STABLE,
    VIBRATING
} PhysicalState;

mpu6050_rst();
mpu6050_init();

int16_t i8to16(uint8_t upper, uint8_t lower);
float convertToAcceleration(int8_t upper, int8_t lower);
float convertToAngularVelocity(int8_t upper, int8_t lower);
float convertToTemperature(int8_t upper, int8_t lower);

PhysicalState checkLinearVibration();
PhysicalState checkAngularVibration();

#endif