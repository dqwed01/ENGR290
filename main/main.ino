/**
 * @file main.ino
 * @author minht
 * @date 2026-09-28
 * @brief Main function
 */
#include "src/hovercraft.h"
#include "src/timer.h"
#include "src/global.h"
#include "src/servo.h"
#include "src/imu.h"
#include "src/uart.h"
 
// ---- Tunable behaviour -----------------------------------------------------
#define SERVO_UPDATE_MS       20UL    // servo angle refresh period
#define REPORT_PERIOD_MS      1500UL  // UART report period
 
int main(){
    hovercraft_init();
 
    float yaw_deg   = 0;
    float roll_deg  = 0;
    float pitch_deg = 0;
    float ax = 0;
    float ay = 0;
    float az = 0;
 
    uint32_t last_servo_ms  = start_ms;
    uint32_t last_report_ms = start_ms;

    float dt = 0;
    float velocityx = 0;
    float positionx = 0;
 
    while(1) {
        uart_tx_pump();
 
        // ---- IMU: read when a fresh sample is available --------------------
        if(imu_read(&yaw_deg, &roll_deg, &pitch_deg, &ax, &ay, &az, &dt)){
            velocityx += ax * dt;
            positionx += velocityx * dt;
        }
 
        uint32_t now = timer_millis();
 
        // ---- Servo: update every 20 ms, no blocking -------------------------
        if((uint32_t)(now - last_servo_ms) >= SERVO_UPDATE_MS){
            last_servo_ms += SERVO_UPDATE_MS;   // fixed period, no drift
            servo_set_angle(yaw_deg);
        }
 
        // ---- UART report: every 1.5 s ---------------------------------------
        if((uint32_t)(now - last_report_ms) >= REPORT_PERIOD_MS &&
           uart_queue_report(roll_deg, pitch_deg, yaw_deg, ax, ay, az, positionx)){
            last_report_ms += REPORT_PERIOD_MS;
        }
    }
 
    return 0;
}
