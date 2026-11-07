#ifndef IMU_H
#define IMU_H

#include <stdint.h>

/*
 * MPU6050 driver for the project's 16 MHz ATmega328P Nano.
 * No Arduino libraries, timers, interrupts, UART, or application globals.
 * The driver owns the AVR TWI peripheral; call it from the main loop, not an ISR.
 * SDA = PC4/A4, SCL = PC5/A5. Use the course board's IMU connector/pull-ups.
 */
enum MPU6050Status {
    MPU6050_OK = 0,
    MPU6050_TIMEOUT,          // A bus operation or fresh-data wait timed out.
    MPU6050_NACK,             // The sensor did not acknowledge an address/byte.
    MPU6050_BUS_ERROR,        // Unexpected AVR TWI hardware status.
    MPU6050_WRONG_ID,         // WHO_AM_I did not contain 0x68.
    MPU6050_BAD_ARGUMENT,
    MPU6050_NOT_INITIALIZED
};

enum MPU6050GyroRange {
    MPU6050_GYRO_250_DPS = 0,
    MPU6050_GYRO_500_DPS,
    MPU6050_GYRO_1000_DPS,
    MPU6050_GYRO_2000_DPS
};

enum MPU6050AccelRange {
    MPU6050_ACCEL_2G = 0,
    MPU6050_ACCEL_4G,
    MPU6050_ACCEL_8G,
    MPU6050_ACCEL_16G
};

struct MPU6050Raw {
    int16_t ax, ay, az;
    int16_t temperature;
    int16_t gx, gy, gz;
};

struct MPU6050Sample {
    float ax_g, ay_g, az_g;       // Sensor-frame acceleration, INCLUDING gravity.
    float gx_dps, gy_dps, gz_dps; // Angular velocity, NOT angles; bias corrected.
    float temperature_c;        // Sensor die temperature, not ambient air.
};

struct MPU6050GyroBias {
    float x_dps, y_dps, z_dps;
};

/*
 * Set up 100 kHz TWI, verify identity, reset/wake the sensor, and configure:
 * +/-250 deg/s, +/-2 g, DLPF=3 (gyro bandwidth 42 Hz), 100 samples/second.
 * Initialization takes about 0.25 seconds. AD0 low -> 0x68; high -> 0x69.
 * WHO_AM_I is 0x68 at EITHER address. One sensor instance is supported.
 */
MPU6050Status mpu6050_init(uint8_t address = 0x68);

/* Settings change both the device registers and the driver's unit conversion.
 * A failed configuration write invalidates initialization: call init() again.
 * Gyro bias is stored in deg/s, so changing range does not rescale the bias.
 * After a setting change, wait for data_ready() before reading the new range.
 */
MPU6050Status mpu6050_set_gyro_range(MPU6050GyroRange range);
MPU6050Status mpu6050_set_accel_range(MPU6050AccelRange range);

/*
 * dlpf: 0..6; 7 is reserved and rejected.
 * Sensor output rate = (dlpf == 0 ? 8000 : 1000) / (1 + divider) Hz.
 * Examples: (3,9) = 100 Hz; (3,255) = 3.90625 Hz; (0,0) = 8000 Hz.
 * These are SENSOR rates, not guaranteed host reading rates. Acceleration has
 * at most 1000 fresh samples/s. This basic driver reads latest data, not FIFO.
 */
MPU6050Status mpu6050_set_sample_rate(uint8_t dlpf, uint8_t divider);
float mpu6050_sample_rate_hz(); // 0 if not initialized.

/* Reading INT_STATUS clears its data-ready flag. A true result means at least
 * one sample arrived; it is not a count of missed samples.
 * Read functions leave the caller's output unchanged on failure.
 */
MPU6050Status mpu6050_data_ready(bool *ready);
MPU6050Status mpu6050_read_raw(MPU6050Raw *sample);
MPU6050Status mpu6050_read(MPU6050Sample *sample);

/*
 * Average fresh gyro readings while the board is COMPLETELY STILL.
 * Default: 200 samples, about 2 s at the default 100 Hz. At 3.90625 Hz this
 * takes about 51 s. Polls with bounded waits; does not use an AVR timer.
 * Constant rotation cannot be distinguished from bias: the caller must keep
 * the board still. Old bias is retained if calibration fails.
 * This calibrates the gyroscope only; it does NOT remove gravity from accel.
 */
MPU6050Status mpu6050_calibrate_gyro(uint16_t samples = 200);
MPU6050GyroBias mpu6050_get_gyro_bias();
void mpu6050_clear_gyro_bias();

/*
 * Simple gravity-based tilt, useful for checking axes on a stationary board.
 * Convention: Z up, roll about X, pitch about Y; results are in degrees.
 * Movement/linear acceleration corrupts these estimates. This is not a fused
 * attitude estimate and cannot measure yaw. Returns false for zero gravity
 * magnitude or when roll is undefined (gravity lies along X).
 */
bool mpu6050_accel_tilt(const MPU6050Sample *sample,
                       float *roll_deg, float *pitch_deg);

#endif