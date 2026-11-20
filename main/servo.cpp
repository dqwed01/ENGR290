#include <avr/io.h>

#include "src/pins.h"
#include "src/servo.h"

void servo_init() {
    TCCR1B = 0;
    TCCR1A = 0;
    PORTB &= ~(1 << SERVO1_CTRL);
    DDRB |= (1 << SERVO1_CTRL);
    TIMSK1 = 0;
    TCNT1 = 0;
    ICR1 = 39999;  // 16 MHz / 8 / 40000 = 50 Hz (20 ms).
    OCR1A = 3000;  // 1500 us neutral pulse, with 0.5 us timer ticks.
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
}

void servo_set_angle(float angle) {
    if (angle > SERVO_MAX_ANGLE_DEG) angle = SERVO_MAX_ANGLE_DEG;
    if (angle < SERVO_MIN_ANGLE_DEG) angle = SERVO_MIN_ANGLE_DEG;
    OCR1A = static_cast<uint16_t>(3000.0f + angle * 20.0f);
}
