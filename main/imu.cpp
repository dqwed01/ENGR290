
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <math.h>

#include "src/imu.h"

namespace {
// MPU6050 register addresses, from the manufacturer's register map.
const uint8_t REG_SAMPLE_DIV = 0x19;
const uint8_t REG_CONFIG = 0x1A;
const uint8_t REG_GYRO_CONFIG = 0x1B;
const uint8_t REG_ACCEL_CONFIG = 0x1C;
const uint8_t REG_INT_ENABLE = 0x38;
const uint8_t REG_INT_STATUS = 0x3A;
const uint8_t REG_ACCEL_X_HIGH = 0x3B;
const uint8_t REG_POWER_1 = 0x6B;
const uint8_t REG_POWER_2 = 0x6C;
const uint8_t REG_WHO_AM_I = 0x75;

uint8_t device_address = 0x68;
bool initialized = false;
float gyro_scale = 131.0f;
float accel_scale = 16384.0f;
uint8_t active_dlpf = 3;
uint8_t active_divider = 9;
MPU6050GyroBias gyro_bias = {0.0f, 0.0f, 0.0f};

// Every hardware wait is bounded. The delay is a CPU busy-wait, so this driver
// does not reconfigure Timer0/1/2 or depend on interrupts being enabled.
MPU6050Status wait_for_twi() {
    for (uint16_t attempt = 0; attempt < 2500; ++attempt) {
        if (TWCR & _BV(TWINT)) return MPU6050_OK;
        _delay_us(2);
    }
    return MPU6050_TIMEOUT;
}

MPU6050Status finish_transaction(MPU6050Status result) {
    // After arbitration loss, release the peripheral without sending STOP on
    // another master's transaction. This application normally has one master.
    if ((TWSR & 0xF8) == TW_MT_ARB_LOST) {
        TWCR = _BV(TWEN);
        return result;
    }
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
    for (uint16_t attempt = 0; attempt < 2500; ++attempt) {
        if (!(TWCR & _BV(TWSTO))) return result;
        _delay_us(2);
    }
    // A stuck bus must not hang the application. Release/reset the peripheral;
    // physical wiring faults still need correction before calling init again.
    TWCR = 0;
    TWCR = _BV(TWEN);
    return result == MPU6050_OK ? MPU6050_TIMEOUT : result;
}

MPU6050Status start_transaction(bool read) {
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    MPU6050Status result = wait_for_twi();
    if (result != MPU6050_OK) return result;
    uint8_t status = TWSR & 0xF8;
    if (status != TW_START && status != TW_REP_START) return MPU6050_BUS_ERROR;

    TWDR = (device_address << 1) | (read ? TW_READ : TW_WRITE);
    TWCR = _BV(TWINT) | _BV(TWEN);
    result = wait_for_twi();
    if (result != MPU6050_OK) return result;
    status = TWSR & 0xF8;
    if (status == (read ? TW_MR_SLA_ACK : TW_MT_SLA_ACK)) return MPU6050_OK;
    if (status == TW_MR_SLA_NACK || status == TW_MT_SLA_NACK) return MPU6050_NACK;
    return MPU6050_BUS_ERROR;
}

MPU6050Status send_byte(uint8_t value) {
    TWDR = value;
    TWCR = _BV(TWINT) | _BV(TWEN);
    MPU6050Status result = wait_for_twi();
    if (result != MPU6050_OK) return result;
    uint8_t status = TWSR & 0xF8;
    if (status == TW_MT_DATA_ACK) return MPU6050_OK;
    return status == TW_MT_DATA_NACK ? MPU6050_NACK : MPU6050_BUS_ERROR;
}

MPU6050Status write_register(uint8_t reg, uint8_t value) {
    MPU6050Status result = start_transaction(false);
    if (result == MPU6050_OK) result = send_byte(reg);
    if (result == MPU6050_OK) result = send_byte(value);
    return finish_transaction(result); // Also release the bus on failure.
}

MPU6050Status read_registers(uint8_t reg, uint8_t *bytes, uint8_t count) {
    MPU6050Status result = start_transaction(false);
    if (result == MPU6050_OK) result = send_byte(reg);
    // Repeated START changes direction without releasing the selected register.
    if (result == MPU6050_OK) result = start_transaction(true);
    for (uint8_t i = 0; i < count && result == MPU6050_OK; ++i) {
        bool last = i == count - 1;
        // ACK intermediate bytes; NACK the final byte to finish the burst.
        TWCR = _BV(TWINT) | _BV(TWEN) | (last ? 0 : _BV(TWEA));
        result = wait_for_twi();
        if (result == MPU6050_OK) {
            if ((TWSR & 0xF8) != (last ? TW_MR_DATA_NACK : TW_MR_DATA_ACK))
                result = MPU6050_BUS_ERROR;
            else
                bytes[i] = TWDR;
        }
    }
    return finish_transaction(result);
}

int16_t signed_word(const uint8_t *bytes) {
    // Sensor sends HIGH byte first; AVR stores integers LOW byte first.
    // Assemble explicitly instead of casting the byte buffer to int16_t*.
    uint16_t value = (static_cast<uint16_t>(bytes[0]) << 8) | bytes[1];
    return value < 0x8000U ? static_cast<int16_t>(value)
                         : static_cast<int16_t>(static_cast<int32_t>(value) - 65536L);
}

MPU6050Status configuration_result(MPU6050Status result) {
    // Clear the old data-ready flag. The caller should wait for the next flag
    // before reading, so an old sample is not interpreted with the new range.
    uint8_t ignored;
    if (result == MPU6050_OK) result = read_registers(REG_INT_STATUS, &ignored, 1);
    // A failed write/STOP could leave hardware partly reconfigured. Do not let
    // subsequent reads silently use conversion factors for the wrong range.
    if (result != MPU6050_OK) initialized = false;
    return result;
}
} // Private names cannot collide with the project's supplied TWI functions.

MPU6050Status mpu6050_init(uint8_t address) {
    if (address != 0x68 && address != 0x69) return MPU6050_BAD_ARGUMENT;
    initialized = false;
    device_address = address;
    // SCL = F_CPU / (16 + 2*TWBR*prescaler). Prescaler=1 gives 100 kHz.
    // Leave GPIO pull-ups alone: the course board must supply suitable pull-ups.
    TWCR = 0;
    TWSR = 0;
    TWBR = static_cast<uint8_t>((F_CPU / 100000UL - 16UL) / 2UL);
    TWCR = _BV(TWEN);
    _delay_ms(100); // Allow sensor power-on before the first transaction.

    uint8_t identity;
    MPU6050Status result = read_registers(REG_WHO_AM_I, &identity, 1);
    if (result != MPU6050_OK) return result;
    if (identity != 0x68) return MPU6050_WRONG_ID;

    result = write_register(REG_POWER_1, 0x80); // Device reset.
    if (result != MPU6050_OK) return result;
    _delay_ms(100);
    const uint8_t settings[][2] = {
        {REG_POWER_1, 0x01},      // Wake, use X gyro PLL as clock source.
        {REG_POWER_2, 0x00},      // Enable all accel/gyro axes.
        {REG_CONFIG, 3},          // DLPF enabled: 42 Hz gyro / 44 Hz accel.
        {REG_SAMPLE_DIV, 9},      // 1000 / (1+9) = 100 Hz.
        {REG_GYRO_CONFIG, 0x00},  // +/-250 deg/s.
        {REG_ACCEL_CONFIG, 0x00}, // +/-2 g.
        {REG_INT_ENABLE, 0x01}    // Enable data-ready status; no INT wire needed.
    };
    for (uint8_t i = 0; i < sizeof(settings) / sizeof(settings[0]); ++i) {
        result = write_register(settings[i][0], settings[i][1]);
        if (result != MPU6050_OK) return result;
    }
    _delay_ms(50); // Let the gyro settle after waking.
    gyro_scale = 131.0f;
    accel_scale = 16384.0f;
    active_dlpf = 3;
    active_divider = 9;
    mpu6050_clear_gyro_bias();
    initialized = true;
    return MPU6050_OK;
}

MPU6050Status mpu6050_set_gyro_range(MPU6050GyroRange range) {
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    if (range < MPU6050_GYRO_250_DPS || range > MPU6050_GYRO_2000_DPS)
        return MPU6050_BAD_ARGUMENT;
    MPU6050Status result = write_register(REG_GYRO_CONFIG, static_cast<uint8_t>(range) << 3);
    if (result == MPU6050_OK) {
        const float scales[] = {131.0f, 65.5f, 32.8f, 16.4f};
        gyro_scale = scales[range];
    }
    return configuration_result(result);
}

MPU6050Status mpu6050_set_accel_range(MPU6050AccelRange range) {
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    if (range < MPU6050_ACCEL_2G || range > MPU6050_ACCEL_16G)
        return MPU6050_BAD_ARGUMENT;
    MPU6050Status result = write_register(REG_ACCEL_CONFIG, static_cast<uint8_t>(range) << 3);
    if (result == MPU6050_OK) {
        const float scales[] = {16384.0f, 8192.0f, 4096.0f, 2048.0f};
        accel_scale = scales[range];
    }
    return configuration_result(result);
}

MPU6050Status mpu6050_set_sample_rate(uint8_t dlpf, uint8_t divider) {
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    if (dlpf > 6) return MPU6050_BAD_ARGUMENT;
    MPU6050Status result = write_register(REG_CONFIG, dlpf);
    if (result == MPU6050_OK) result = write_register(REG_SAMPLE_DIV, divider);
    if (result == MPU6050_OK) {
        active_dlpf = dlpf;
        active_divider = divider;
    }
    return configuration_result(result);
}

float mpu6050_sample_rate_hz() {
    if (!initialized) return 0.0f;
    return (active_dlpf == 0 ? 8000.0f : 1000.0f) / (1.0f + active_divider);
}

MPU6050Status mpu6050_data_ready(bool *ready) {
    if (!ready) return MPU6050_BAD_ARGUMENT;
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    uint8_t status;
    MPU6050Status result = read_registers(REG_INT_STATUS, &status, 1);
    if (result == MPU6050_OK) *ready = (status & 0x01) != 0;
    return result;
}

MPU6050Status mpu6050_read_raw(MPU6050Raw *sample) {
    if (!sample) return MPU6050_BAD_ARGUMENT;
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    uint8_t bytes[14];
    MPU6050Status result = read_registers(REG_ACCEL_X_HIGH, bytes, sizeof(bytes));
    if (result != MPU6050_OK) return result;
    // One burst keeps all seven values from the same sensor sampling instant.
    MPU6050Raw value = {signed_word(bytes), signed_word(bytes + 2),
        signed_word(bytes + 4), signed_word(bytes + 6), signed_word(bytes + 8),
        signed_word(bytes + 10), signed_word(bytes + 12)};
    *sample = value;
    return MPU6050_OK;
}

MPU6050Status mpu6050_read(MPU6050Sample *sample) {
    if (!sample) return MPU6050_BAD_ARGUMENT;
    MPU6050Raw raw;
    MPU6050Status result = mpu6050_read_raw(&raw);
    if (result != MPU6050_OK) return result;
    sample->ax_g = raw.ax / accel_scale;
    sample->ay_g = raw.ay / accel_scale;
    sample->az_g = raw.az / accel_scale;
    sample->gx_dps = raw.gx / gyro_scale - gyro_bias.x_dps;
    sample->gy_dps = raw.gy / gyro_scale - gyro_bias.y_dps;
    sample->gz_dps = raw.gz / gyro_scale - gyro_bias.z_dps;
    sample->temperature_c = raw.temperature / 340.0f + 36.53f;
    return MPU6050_OK;
}

MPU6050Status mpu6050_calibrate_gyro(uint16_t samples) {
    if (!initialized) return MPU6050_NOT_INITIALIZED;
    if (samples == 0) return MPU6050_BAD_ARGUMENT;
    bool ready;
    MPU6050Status result = mpu6050_data_ready(&ready); // Discard any old flag.
    if (result != MPU6050_OK) return result;
    int32_t sum_x = 0, sum_y = 0, sum_z = 0;
    for (uint16_t i = 0; i < samples; ++i) {
        ready = false;
        for (uint16_t wait_ms = 0; wait_ms < 1000; ++wait_ms) {
            result = mpu6050_data_ready(&ready);
            if (result != MPU6050_OK) return result;
            if (ready) break;
            _delay_ms(1);
        }
        if (!ready) return MPU6050_TIMEOUT;
        MPU6050Raw raw;
        result = mpu6050_read_raw(&raw);
        if (result != MPU6050_OK) return result;
        sum_x += raw.gx;
        sum_y += raw.gy;
        sum_z += raw.gz;
    }
    // Commit all three offsets only after a complete successful calibration.
    float divisor = static_cast<float>(samples) * gyro_scale;
    gyro_bias.x_dps = sum_x / divisor;
    gyro_bias.y_dps = sum_y / divisor;
    gyro_bias.z_dps = sum_z / divisor;
    return MPU6050_OK;
}

MPU6050GyroBias mpu6050_get_gyro_bias() { return gyro_bias; }

void mpu6050_clear_gyro_bias() {
    gyro_bias.x_dps = gyro_bias.y_dps = gyro_bias.z_dps = 0.0f;
}

bool mpu6050_accel_tilt(const MPU6050Sample *sample, float *roll_deg, float *pitch_deg) {
    if (!sample || !roll_deg || !pitch_deg) return false;
    float yz_squared = sample->ay_g * sample->ay_g + sample->az_g * sample->az_g;
    if (yz_squared < 0.000001f) return false;
    const float radians_to_degrees = 57.2957795f;
    *roll_deg = atan2f(sample->ay_g, sample->az_g) * radians_to_degrees;
    *pitch_deg = atan2f(-sample->ax_g, sqrtf(yz_squared)) * radians_to_degrees;
    return true;
}