#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define SERVO_PWM_TOP 39999
#define SERVO_PWM_CENTER 3000   // OCR1A for servo center: 3000 × 0.5 µs = 1.5 ms pulse
#define FAN_PWM_TOP 79   // Timer2 TOP (OCR2A): 2 MHz / (79 + 1) = 25 kHz

void pwm_init(void);
void pwm_set_servo_angle(float angle);
void pwm_set_fan_speed(uint8_t percent);   // 0-100

#endif
