/**
 * @file main.c
 * @author minht
 * @date 2026-09-28
 * @brief Main function
 */
#include <avr/io.h>
#include <math.h>
 
#include "src/hovercraft.h"
#include "src/util.h"
#include "src/timer.h"
#include "src/pwm.h"
#include "src/global.h"
#include "src/servo.h"
#include "src/imu.h"
#include "src/uart.h"
 
// ---- Tunable behaviour -----------------------------------------------------
#define YAW_LED_THRESHOLD_DEG 85.0f   // PB5 LED on when |yaw| exceeds this
#define PWM_ACCEL_OFF_G       0.11f   // |ax| below this  -> 0 %   duty
#define PWM_ACCEL_FULL_G      1.11f   // |ax| above this  -> 100 % duty
#define SERVO_UPDATE_MS       20UL    // servo angle refresh period
 
int main(){
    hovercraft_init();
    uart_report_init(start_ms);
 
    float yaw_deg   = 0;
    float roll_deg  = 0;
    float pitch_deg = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
 
    uint32_t last_servo_ms  = start_ms;

    float dt = 0;
    float velocityx = 0;
    float positionx = 0;
 
    while(1) {
        // ---- IMU: read when a fresh sample is available --------------------
        if(imu_read(&yaw_deg, &roll_deg, &pitch_deg, &ax, &ay, &az, &dt)){
            // PB5 LED: on while |yaw| > 85 degrees, off otherwise.
            if(fabsf(yaw_deg) > YAW_LED_THRESHOLD_DEG){
                DIGITAL_WRITE_HIGH(PORTB, PB5);
            }
            else{
                DIGITAL_WRITE_LOW(PORTB, PB5);
            }

            velocityx += ax * dt;
            positionx += velocityx * dt;
            // PWM LED brightness from |X acceleration|:
            //   < 0.11 g -> 0 %,  > 1.11 g -> 100 %,  linear in between.
            ax = fabsf(ax);

            uint8_t pwm_duty;
            if(ax < PWM_ACCEL_OFF_G){
                pwm_duty = 0;
            }
            else if(ax > PWM_ACCEL_FULL_G){
                pwm_duty = 100;
            }
            else{
                pwm_duty = (uint8_t)(((ax - PWM_ACCEL_OFF_G) /
                            (PWM_ACCEL_FULL_G - PWM_ACCEL_OFF_G)) * 100.0f + 0.5f);
            }
            pwm_set_duty(pwm_duty);
        }
 
        uint32_t now = timer_millis();
 
        // ---- Servo: update every 20 ms, no blocking -------------------------
        if((uint32_t)(now - last_servo_ms) >= SERVO_UPDATE_MS){
            last_servo_ms += SERVO_UPDATE_MS;   // fixed period, no drift
            servo_set_angle(yaw_deg);
        }
 
        uart_report_update(now, roll_deg, pitch_deg, yaw_deg,
                           ax, ay, az, positionx);
    }
 
    return 0;
}
