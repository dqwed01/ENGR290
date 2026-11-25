#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

#include "src/hovercraft.h"
#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"
#include "src/infrared.h"
#include "src/pwm.h"
#include "src/global.h"
#include "src/servo.h"
#include "src/imu.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

static hovercraft_modes mode = HOVERCRAFT_MOVING;
static void adjust_thrust_fan(hovercraft_modes, float, float, float);
static void is_turn_condition(float, float);
static void is_move_condition(float, float);
static void get_sensor_data()


void hovercraft_init(){
    cli();
    UART_init();
    pwm_init();
    timer_init();
    us_sensor_init();
    adc_init();
    servo_init();
    mpu6050_init();
    mpu6050_set_gyro_range(MPU6050_GYRO_250_DPS);
    mpu6050_set_accel_range(MPU6050_ACCEL_2G);
    mpu6050_set_sample_rate(3,9);
    _delay_ms(2000);
    mpu6050_calibrate_gyro(500);
    start_ms = timer_millis();
    sei();
}

void hovercraft_act(){
    float h_ifr_distance[2] = {0};
    float us_distance = 0;
    float yaw_deg   = 0;
    float roll_deg  = 0;
    float pitch_deg = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
    float dt = 0;

    imu_read(&yaw_deg, &roll_deg, &pitch_deg, &ax, &ay, &az, &dt);
    h_ifr_distance[0] = ifr_read(0);
    h_ifr_distance[1] = ifr_read(1);
    us_distance = us_read();

    uart_report_update(timer_millis(), roll_deg, pitch_deg, yaw_deg, ax, ay, az, h_ifr_distance[0], h_ifr_distance[1], us_distance);
    if(us_distance <= BAR_DISTANCE_CM){
        //Add code to turn off Lift Fan here
        delay_ms(5000);
        return;
    }
    switch(mode){
        case HOVERCRAFT_MOVING:
            
            break;

        case HOVERCRAFT_TURNING:
            break;
    }
}