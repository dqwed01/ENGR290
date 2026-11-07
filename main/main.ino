/**
 * @file main.c
 * @author minht
 * @date 2026-09-28
 * @brief Main function
 */
#define F_CPU 16000000UL
 
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
 
#include "src/hovercraft.h"
#include "src/util.h"
#include "src/timer.h"
#include "src/pwm.h"
#include "src/global.h"
#include "src/servo.h"
#include "src/imu.h"
 
// ---- Tunable behaviour -----------------------------------------------------
#define YAW_LED_THRESHOLD_DEG 85.0f   // PB5 LED on when |yaw| exceeds this
#define PWM_ACCEL_OFF_G       0.11f   // |ax| below this  -> 0 %   duty
#define PWM_ACCEL_FULL_G      1.11f   // |ax| above this  -> 100 % duty
#define SERVO_UPDATE_MS       20UL    // servo angle refresh period
#define REPORT_PERIOD_MS      1500UL  // UART report period
 
// ---- Non-blocking UART transmit -------------------------------------------
// UART_transmit() busy-waits (~1 ms per character at 9600 baud), so a ~70
// character report would stall the loop for ~70 ms and make the IMU/servo
// miss their timing. Instead the report is formatted into a buffer and the
// main loop hands one byte to the UART whenever its data register is empty.
static char    tx_buf[144];
static uint8_t tx_len = 0;
static uint8_t tx_pos = 0;
 
static inline bool uart_tx_busy(){
    return tx_pos < tx_len;
}
 
static inline void uart_tx_pump(){
    if(tx_pos < tx_len && (UCSR0A & (1 << UDRE0))){
        UDR0 = tx_buf[tx_pos++];
    }
}
 
static void uart_queue_report(float roll, float pitch, float yaw,
                              float ax, float ay, float az, float positionx){
    char r[12], p[12], y[12], x[12], yy[12], z[12], posx[12];
    dtostrf(roll,  1, 2, r);
    dtostrf(pitch, 1, 2, p);
    dtostrf(yaw,   1, 2, y);
    dtostrf(ax,    1, 2, x);
    dtostrf(ay,    1, 2, yy);
    dtostrf(az,    1, 2, z);
    dtostrf(positionx,  1, 2, posx);
    int n = snprintf(tx_buf, sizeof(tx_buf),
                     "roll=%s pitch=%s yaw=%s deg | ax=%s ay=%s az=%s | postitionx=%s g\r\n",
                     r, p, y, x, yy, z, posx);
    if(n < 0) n = 0;
    if(n > (int)sizeof(tx_buf) - 1) n = sizeof(tx_buf) - 1;
    tx_len = (uint8_t)n;
    tx_pos = 0;
}
 
int main(){
    hovercraft_init();
 
    MPU6050Sample s = {0, 0, 0, 0, 0, 0, 0};
    float yaw_deg   = 0;
    float roll_deg  = 0;
    float pitch_deg = 0;
 
    uint32_t start_ms       = timer_millis();
    uint32_t last_imu_ms    = start_ms;
    uint32_t last_servo_ms  = start_ms;
    uint32_t last_report_ms = start_ms;

    float dt = 0;
    float velocityx = 0;
    float positionx = 0;
 
    while(1) {
        uart_tx_pump();
 
        // ---- IMU: read when a fresh sample is available --------------------
        bool ready = false;
        if(mpu6050_data_ready(&ready) == MPU6050_OK && ready){
            if(mpu6050_read(&s) == MPU6050_OK){
                // Integrate gyro Z using the real time since the last sample.
                uint32_t imu_now = timer_millis();
                dt = (imu_now - last_imu_ms) *0.001f;
                yaw_deg += s.gz_dps * dt;
                last_imu_ms = imu_now;
 
                float r, p;
                if(mpu6050_accel_tilt(&s, &r, &p)){
                    roll_deg  = r;
                    pitch_deg = p;
                }
 
                // PB5 LED: on while |yaw| > 85 degrees, off otherwise.
                if(fabsf(yaw_deg) > YAW_LED_THRESHOLD_DEG){
                    DIGITAL_WRITE_HIGH(PORTB, PB5);
                }
                else{
                    DIGITAL_WRITE_LOW(PORTB, PB5);
                }
 
                velocityx += s.ax_g * dt;
                positionx += velocityx * dt;
                // PWM LED brightness from |X acceleration|:
                //   < 0.11 g -> 0 %,  > 1.11 g -> 100 %,  linear in between.
                float ax = fabsf(s.ax_g);

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
        }
 
        uint32_t now = timer_millis();
 
        // ---- Servo: update every 20 ms, no blocking -------------------------
        if((uint32_t)(now - last_servo_ms) >= SERVO_UPDATE_MS){
            last_servo_ms += SERVO_UPDATE_MS;   // fixed period, no drift
            servo_set_angle(yaw_deg);
        }
 
        // ---- UART report: every 1.5 s ---------------------------------------
        if((uint32_t)(now - last_report_ms) >= REPORT_PERIOD_MS && !uart_tx_busy()){
            last_report_ms += REPORT_PERIOD_MS;
            uart_queue_report(roll_deg, pitch_deg, yaw_deg,
                              s.ax_g, s.ay_g, s.az_g, positionx);
        }
    }
 
    return 0;
}
