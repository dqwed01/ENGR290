#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/hovercraft.h"
#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"
#include "src/infrared.h"
#include "src/pwm.h"
#include "src/servo.h"
#include "src/global.h"
#include "src/imu.h"

void hovercraft_init(){
    cli();
    UART_init();
    pwm_init();
    servo_init();
    timer_init();
    us_sensor_init();
    adc_init();
    mpu6050_init();
    mpu6050_set_gyro_range(MPU6050_GYRO_250_DPS);
    mpu6050_set_accel_range(MPU6050_ACCEL_2G);
    mpu6050_set_sample_rate(3,9);
    _delay_ms(2000);
    mpu6050_calibrate_gyro(500);
    start_ms = timer_millis();
    sei();
}