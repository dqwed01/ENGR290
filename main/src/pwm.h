#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define SERVO_PWM_TOP 39999
#define SERVO_PWM_CENTER 3000   // OCR1A for servo center: 3000 × 0.5 µs = 1.5 ms pulse
#define FAN_PWM_TOP 255   // Timer2 Phase Correct mode with TOP = 255: 16 MHz / (2*255) = 31.4 kHz

void pwm_init(void);
void pwm_set_servo_angle(float angle);
void pwm_set_fan_speed(volatile uint8_t* OCR2x, uint8_t percent);

#endif
