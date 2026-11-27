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
#include "src/pins.h"
#include "src/servo.h"
#include "src/global.h"
#include "src/imu.h"
#include "src/uart.h"
#include "src/ultrasonic.h"
#include "src/infrared.h"
#include <util/delay.h>
 
// ---- Tunable behaviour -----------------------------------------------------
#define PWM_ACCEL_OFF_G       0.11f   // |ax| below this  -> 0 %   duty
#define PWM_ACCEL_FULL_G      1.11f   // |ax| above this  -> 100 % duty
#define SERVO_UPDATE_MS       20UL    // servo angle refresh period
 
int main(){
    hovercraft_init();
    uart_report_init(start_ms);

    pwm_set_duty(&OCR0A, 255, 80); // set lift fan
    pwm_set_duty(&OCR0B, 255, 10); // set thrust fan
 
    while(1) {
        hovercraft_act();
        // uart_report_update(now, roll_deg, pitch_deg, yaw_deg,
        //                    ax, ay, az, positionx);
        // _delay_ms(5000);
    }
 
    return 0;
}
