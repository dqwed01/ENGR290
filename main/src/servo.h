#ifndef SERVO_H
#define SERVO_H

// Timer1 Fast PWM, TOP = ICR1, prescaler 8 -> one tick = 0.5 µs
#define SERVO_PWM_TOP     39999   // (39999 + 1) × 0.5 µs = 20 ms period
#define SERVO_PWM_CENTER  3000    // 3000 × 0.5 µs = 1.5 ms pulse (center)
#define SERVO_MAX_ANGLE   80.0f   // mechanical limit, degrees either side of center
#define SERVO_TICKS_PER_DEG 20.0f // 20 ticks = 10 µs per degree

void servo_init(void);
void servo_set_angle(float angle);   // -SERVO_MAX_ANGLE to +SERVO_MAX_ANGLE

#endif
