#include "MPU6050_reg.h"
#include "MPU6050.h"

#define MPU6050_SENSITIVITY 16384.0  // Sensitivity scale factor for ±2G range
#define GRAVITY 9.81                 // Acceleration due to gravity in m/s^2

#define MPU6050_GYRO_SENSITIVITY 65.5  // Sensitivity scale factor for ±500 °/s range

#define MPU6050_TEMP_SENSITIVITY 340.0  // Sensitivity scale factor for temperature
#define MPU6050_TEMP_OFFSET 36.53       // Temperature offset in degrees Celsius
mpu6050_rst(void)
{
    //Reset Sequence
    MXC_DELAY_MSEC(10);
    write_bitslice(MPU6050_PWR_MGMT_1,0x01,1,7);
    MXC_DELAY_MSEC(10);
    write_req(MPU6050_SIGNAL_PATH_RESET,0x07);
    MXC_DELAY_MSEC(100);
}

mpu6050_init(void)
{
    //Initialization Sequence
    //setSampleRateDivisor
    write_req(MPU6050_SMPLRT_DIV,0x00);

    //setFilterBandWidth
    write_bitslice(MPU6050_CONFIG,0x00,3,0);

    //setGyroRange
    write_bitslice(MPU6050_GYRO_CONFIG,0x01,2,3);

    //setAccelerometerRange
    write_bitslice(MPU6050_ACCEL_CONFIG,0x00,2,3);

    //set clock config to PLL with Gyro X reference
    write_req(MPU6050_PWR_MGMT_1,0x01);
}

// Accelerometer Data
uint8_t accel_x_h(void){return read_req(0x3B);}

uint8_t accel_x_l(void){return read_req(0x3C);}

uint8_t accel_y_h(void){return read_req(0x3D);}

uint8_t accel_y_l(void){return read_req(0x3E);}

uint8_t accel_z_h(void){return read_req(0x3F);}

uint8_t accel_z_l(void){return read_req(0x40);}

// Gyroscope Data
uint8_t gyro_x_h(void){return read_req(0x43);}

uint8_t gyro_x_l(void){return read_req(0x44);}

uint8_t gyro_y_h(void){return read_req(0x45);}

uint8_t gyro_y_l(void){return read_req(0x46);}

uint8_t gyro_z_h(void){return read_req(0x47);}

uint8_t gyro_z_l(void){return read_req(0x48);}

// Temperature Data
uint8_t temperature_h(void){return read_req(0x41);}

uint8_t temperature_l(void){return read_req(0x42);}

float convertToAcceleration(int8_t upper, int8_t lower) {
    // Combine the upper and lower bytes
    int16_t rawValue = (upper << 8) | (uint8_t)lower;

    // Convert to g's by dividing by sensitivity
    float acceleration_g = rawValue / MPU6050_SENSITIVITY;

    // Convert to m/s^2 by multiplying by gravity
    float acceleration_mps2 = acceleration_g * GRAVITY;

    return acceleration_mps2;
}

float convertToAngularVelocity(int8_t upper, int8_t lower) {
    // Combine the upper and lower bytes
    int16_t rawValue = (upper << 8) | (uint8_t)lower;

    // Convert to degrees per second by dividing by sensitivity
    float angularVelocity_dps = rawValue / MPU6050_GYRO_SENSITIVITY;

    return angularVelocity_dps;
}

float convertToTemperature(int8_t upper, int8_t lower) {
    // Combine the upper and lower bytes
    int16_t rawValue = (upper << 8) | (uint8_t)lower;

    // Convert to degrees Celsius using the formula
    float temperature_C = (rawValue / MPU6050_TEMP_SENSITIVITY) + MPU6050_TEMP_OFFSET;

    return temperature_C;
}