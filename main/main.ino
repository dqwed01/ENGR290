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
        hovercraft_act();
        uart_report_update(now, roll_deg, pitch_deg, yaw_deg,
                           ax, ay, az, positionx);
    }
 
    return 0;
}
