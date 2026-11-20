#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

// Bench travel about neutral. Actual horn angle depends on servo calibration.
constexpr int8_t SERVO_MIN_ANGLE_DEG = -40;
constexpr int8_t SERVO_MAX_ANGLE_DEG = 40;

// P9 pin 1, PB1/OC1A. Timer1 is independent of the lift PWM timer.
void servo_init();
void servo_set_angle(float angle);

#endif
